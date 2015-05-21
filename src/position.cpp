/*!
 * \file position.cpp
 * \brief Definition for performing RTK positioning.
 * \author Anthony Arnold, 2015. anthony.arnold(at)uqconnect.edu.au
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) Anthony Arnold 2015
 *
 * Genesis is a realtime multi-station GNSS receiver.
 *
 * This file is part of Genesis.
 *
 * Genesis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Genesis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Genesis. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#include <map>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/move/core.hpp>
#include "position.hpp"
#include "rtklib.h"
#include "log.hpp"
#include "gps_data.hpp"
#include "client_controller.hpp"
#include <boost/date_time/posix_time/posix_time_types.hpp>

#define TWO_PI 6.28318530718

namespace genesis {

struct rtk_t : ::rtk_t {};

namespace detail {

void to_gtime_t (double gps_t, int week, gtime_t &out) {
    // convert to time since gps rollover
    double ms = (gps_t + 604800 * static_cast<double>(week % 1024)) * 1000;

    boost::posix_time::time_duration t = boost::posix_time::millisec(ms);

    out.time = t.total_seconds ();
    out.sec = t.fractional_seconds ();
}

void get_obs (const std::vector <gnss_sdr_data> &observables,
              bool base,
              const Gps_Ref_Time &ref_time,
              std::vector <obsd_t> &out)
{
    std::map <unsigned int, obsd_t> rtkobs;
    typedef std::map<unsigned int, obsd_t>::value_type obs_pair;

    BOOST_FOREACH (const gnss_sdr_data &data, observables) {

        // Convert the GNSS-SDR observable data to the RTKLIB
        // observable o
        obsd_t &o = rtkobs[data.PRN];
        to_gtime_t (ref_time.d_TOW, ref_time.d_Week, o.time);
        o.sat = (unsigned char)data.PRN;
        o.rcv = base ? 2 : 1;
        o.code [0] = CODE_L1C;
        o.L[0] = data.Carrier_phase_rads / TWO_PI; // radians to cycles
        o.P[0] = data.Pseudorange_m;
        o.D[0] = data.Carrier_Doppler_hz;
    }

    BOOST_FOREACH (const obs_pair pair, rtkobs) {
        out.push_back (pair.second);
    }
}

} // namespace detail


position::position (controller_ptr controller, gps_data_ptr gps)
    : controller_ (controller), gps_data_ (gps), rtk_(new rtk_t)
{
    prcopt_t options = prcopt_default;

    // TODO: Allow more options
    options.mode = PMODE_FIXED; // Fixed base station
    options.nf = 1; // GPS L1

    rtkinit (rtk_.get (), &options);
}

position::~position () {
    rtkfree (rtk_.get ());
}

position::error_type position::rtk_position (
    const std::vector <gnss_sdr_data> &observables)
{
    if (!controller_->has_base ()) {
        return make_error_condition (no_base_station);
    }

    // Copy GNSS-SDR Observables to RTKLIB observables (observations)
    // get_obs pushes in PRN order
    // rtklib requires in order of receiver, followed by satellite

    // BASE STATION OBSERVABLES
    std::vector <obsd_t> observations;
    Gps_Ref_Time ref_time;
    controller_->base_ref_time ()->read (0, ref_time);
    const std::vector <gnss_sdr_data> &base_observables =
       controller_->base_observables ();

    detail::get_obs (base_observables,
                     true,
                     ref_time,
                     observations);

    // ROVER OBSERVABLES
    gps_data_->ref_time()->read (0, ref_time);
    detail::get_obs (observables, false, ref_time, observations);

    // Set up navigation data
    nav_t nav;
    memset (&nav, 0, sizeof (nav));

    // Convert GNSS-SDR ephemeris to RTKLIB ephemeris
    std::vector <eph_t> ephemeris;
    std::map <int, Gps_Ephemeris> ephms =
       gps_data_->ephemeris()->get_map_copy ();

    typedef  std::map <int, Gps_Ephemeris>::value_type eph_pair;
    BOOST_FOREACH (const eph_pair &e, ephms) {
        eph_t eph;
        const Gps_Ephemeris &dat = e.second;

        eph.sat = dat.i_satellite_PRN;
        eph.iodc = dat.d_IODC;
        eph.iode = dat.d_IODE_SF2; // GNSS-SDR validates this
        eph.sva = dat.i_SV_accuracy;
        eph.svh = dat.i_SV_health;
        eph.week = dat.i_GPS_week;
        eph.code = dat.i_code_on_L2;
        eph.flag = (int)dat.b_L2_P_data_flag;
        detail::to_gtime_t (dat.d_Toe, dat.i_GPS_week, eph.toe);
        detail::to_gtime_t (dat.d_Toc, dat.i_GPS_week, eph.toc);

        // correct clock
        double dt =  dat.d_TOW - dat.d_Toc;
        static const double half_week = 302400;     // seconds
        if (dt > half_week)
        {
            dt = dt - 2 * half_week;
        }
        else if (dt < -half_week)
        {
            dt = dt + 2 * half_week;
        }
        double corr =
           (dat.d_A_f2 * dt + dat.d_A_f1) * dt + dat.d_A_f0 + dat.d_dtr;
        corr = dat.d_TOW - corr;
        detail::to_gtime_t (corr, dat.i_GPS_week, eph.ttr);

        // Orbital parameters
        eph.A = (dat.d_sqrt_A * dat.d_sqrt_A);
        eph.e = dat.d_e_eccentricity;
        eph.i0 = dat.d_i_0;
        eph.OMG0 = dat.d_OMEGA0;
        eph.omg = dat.d_OMEGA;
        eph.M0 = dat.d_M_0;
        eph.deln = dat.d_Delta_n;
        eph.OMGd = dat.d_OMEGA_DOT;
        eph.idot = dat.d_IDOT;

        eph.crc = dat.d_Crc;
        eph.cic = dat.d_Cic;
        eph.cis = dat.d_Cis;
        eph.cus = dat.d_Cus;
        eph.crs = dat.d_Crs;
        eph.cuc = dat.d_Cuc;

        eph.toes = dat.d_TOW;
        eph.fit = dat.b_fit_interval_flag;
        eph.f0 = dat.d_A_f0;
        eph.f1 = dat.d_A_f1;
        eph.f2 = dat.d_A_f2;

        eph.tgd[0] = dat.d_TGD;

        ephemeris.emplace_back (boost::move (eph));
    }
    nav.eph = &ephemeris[0];
    nav.n = nav.nmax = ephemeris.size ();

    // Convert GNSS-SDR almanac objects to RTKLIB almanacs
    std::vector <alm_t> almanac;
    std::map <int, Gps_Almanac> alms = gps_data_->almanac()->get_map_copy ();
    typedef  std::map <int, Gps_Almanac>::value_type alm_pair;
    BOOST_FOREACH (const alm_pair &a, alms) {
        alm_t alm;
        const Gps_Almanac &dat = a.second;

        alm.sat = dat.i_satellite_PRN;
        alm.svh = dat.i_SV_health;
        alm.svconf = 0; // fixme
        alm.week = ref_time.d_Week;
        alm.toa.time = dat.d_Toa;
        alm.toa.sec = (dat.d_Toa - (time_t)dat.d_Toa) / 1000000.0;
        detail::to_gtime_t (dat.d_Toa, ref_time.d_Week, alm.toa);
        alm.A = (dat.d_sqrt_A * dat.d_sqrt_A);
        alm.e = dat.d_e_eccentricity;
        alm.i0 = 0; // fixme
        alm.OMG0 = dat.d_OMEGA0;
        alm.omg = dat.d_OMEGA;
        alm.M0 = dat.d_M_0;
        alm.OMGd = dat.d_OMEGA_DOT;
        alm.toas = dat.d_Toa;
        alm.f0 = dat.d_A_f0;
        alm.f1 = dat.d_A_f1;

        almanac.emplace_back (boost::move (alm));
    }
    nav.alm = &almanac[0];
    nav.na = nav.namax = almanac.size ();

    // Convert UTC time params from GNSS-SDR to RTKLIB
    Gps_Utc_Model utc;
    if (gps_data_->utc_model()->read (0, utc)) {
        if (utc.valid) {
            nav.utc_gps[0] = utc.d_A0;
            nav.utc_gps[1] = utc.d_A1;
            nav.utc_gps[2] = utc.d_t_OT;
            nav.utc_gps[3] = utc.i_WN_T;
            nav.leaps = utc.d_DeltaT_LS;
        }
    }

    // Convert ionospheric model from GNSS-SDR to RTKLIB
    Gps_Iono iono;
    if (gps_data_->iono()->read (0, iono)) {
        if (iono.valid) {
            nav.ion_gps[0] = iono.d_alpha0;
            nav.ion_gps[1] = iono.d_alpha1;
            nav.ion_gps[2] = iono.d_alpha2;
            nav.ion_gps[3] = iono.d_alpha3;
            nav.ion_gps[4] = iono.d_beta0;
            nav.ion_gps[5] = iono.d_beta1;
            nav.ion_gps[6] = iono.d_beta2;
            nav.ion_gps[7] = iono.d_beta3;
        }
    }

    // Ready to run
    int rv = rtkpos (rtk_.get (), &observations[0], observations.size (), &nav);
    if (!rv) {
        return make_error_condition (rtk_failure);
    }

    // Got valid position
    // TODO: Print to KML somewhere
    BOOST_LOG_SEV (lg_, debug)
       << "Got valid position for station "
       << gps_data_->name();

    BOOST_LOG_SEV (lg_, info)
       << gps_data_->name() << ": ("
       << rtk_->sol.rr[0] << ", "
       << rtk_->sol.rr[1] << ", "
       << rtk_->sol.rr[2] << ") ("
       << rtk_->sol.rr[3] << ", "
       << rtk_->sol.rr[4] << ", "
       << rtk_->sol.rr[5] << ")";

    BOOST_LOG_SEV (lg_, info)
       << "Base: ("
       << rtk_->rb[0] << ", "
       << rtk_->rb[1] << ", "
       << rtk_->rb[2] << ") ("
       << rtk_->rb[3] << ", "
       << rtk_->rb[4] << ", "
       << rtk_->rb[5] << ")";

    return error_type ();
}

}
