#ifndef RKCORE_H
#define RKCORE_H

#ifdef __cplusplus
extern "C"
{
#endif
	size_t __rkcore_MOD_init_rkc ( void );
	void __rkcore_MOD_uinit_rkc ( const size_t );
	size_t __rkcore_MOD_cp_rkc ( const size_t );
	void __rkccore_MOD_clear_stat ( const size_t );
	void __rkccore_MOD_set_print_error ( const size_t, const bool );
///
	void __rkcore_MOD_set_param ( const size_t, const double, const double );
	void __rkcore_MOD_set_eunit ( const size_t, const int );
	void __rkcore_MOD_set_aunit ( const size_t, const int );
	void __rkcore_MOD_set_mass ( const size_t, const double, const double, const double, const double );
	double __rkcore_MOD_get_set_mass ( const size_t, const size_t, const size_t, const double );
	double __rkcore_MOD_get_set_strip_mass ( const size_t, const size_t, const double );
	void __rkcore_MOD_set_ex ( const size_t, const double );
	void __rkcore_MOD_set_k1 ( const size_t, const double );
	void __rkcore_MOD_set_k1c ( const size_t, const double );
	void __rkcore_MOD_set_p1 ( const size_t, const double );
	void __rkcore_MOD_set_p1c ( const size_t, const double );
	void __rkcore_MOD_set_k34c ( const size_t );
	void __rkcore_MOD_set_th3max ( const size_t );
	void __rkcore_MOD_set_k3sign ( const size_t, const int );
	int __rkcore_MOD_set_theta3 ( const size_t, const double );
	int __rkcore_MOD_set_theta3c ( const size_t, const double );
	int __rkcore_MOD_set_q ( const size_t, const double );
///
	double __rkcore_MOD_get_qvalue ( const size_t );
	double __rkcore_MOD_get_k1min ( const size_t );
	double __rkcore_MOD_get_k1cmin ( const size_t );
	double __rkcore_MOD_get_p1min ( const size_t );
	double __rkcore_MOD_get_p1cmin ( const size_t );

	double __rkcore_MOD_get_k1 ( const size_t );
	double __rkcore_MOD_get_k3 ( const size_t );
	double __rkcore_MOD_get_k4 ( const size_t );

	double __rkcore_MOD_get_k1c ( const size_t );
	double __rkcore_MOD_get_k2c ( const size_t );
	double __rkcore_MOD_get_k3c ( const size_t );
	double __rkcore_MOD_get_k4c ( const size_t );

	double __rkcore_MOD_get_p1 ( const size_t );
	double __rkcore_MOD_get_p3 ( const size_t );
	double __rkcore_MOD_get_p4 ( const size_t );

	double __rkcore_MOD_get_p1c ( const size_t );
	double __rkcore_MOD_get_p3c ( const size_t );
	double __rkcore_MOD_get_p4c ( const size_t );

	double __rkcore_MOD_get_e1 ( const size_t );
	double __rkcore_MOD_get_e3 ( const size_t );
	double __rkcore_MOD_get_e4 ( const size_t );

	double __rkcore_MOD_get_e1c ( const size_t );
	double __rkcore_MOD_get_e2c ( const size_t );
	double __rkcore_MOD_get_e3c ( const size_t );
	double __rkcore_MOD_get_e4c ( const size_t );

	double __rkcore_MOD_get_beta ( const  size_t );
	double __rkcore_MOD_get_gamma ( const size_t );
	double __rkcore_MOD_get_betagamma ( const size_t );
	double __rkcore_MOD_get_invgamma ( const size_t );

	double __rkcore_MOD_get_qmax ( const size_t );
	double __rkcore_MOD_get_qmin ( const size_t );

	double __rkcore_MOD_get_exmax ( const size_t );
	double __rkcore_MOD_get_ex ( const size_t );

	double __rkcore_MOD_get_th3 ( const size_t );
	double __rkcore_MOD_get_th4 ( const size_t );
	double __rkcore_MOD_get_th3c ( const size_t );
	double __rkcore_MOD_get_th4c ( const size_t );
	double __rkcore_MOD_get_th3max ( const size_t );
	double __rkcore_MOD_get_th3cmax ( const size_t );

	double __rkcore_MOD_get_q ( const size_t );

	double __rkcore_MOD_get_j3 ( const size_t );
	double __rkcore_MOD_get_j4 ( const size_t );

	double __rkcore_MOD_get_kshift ( const size_t );
	double __rkcore_MOD_get_kfactor ( const size_t );

	bool __rkcore_MOD_get_flag_invkin ( const size_t );
	bool __rkcore_MOD_get_flag_th3maxne ( const size_t );
#ifdef __cplusplus
}
#endif

#define RKCERR_RANGE_UNDER 1
#define RKCERR_RANGE_OVER  2

#define rkc_init  __rkcore_MOD_init_rkc
#define rkc_uinit  __rkcore_MOD_uinit_rkc
#define rkc_cp __rkcore_MOD_cp_rkc
#define rkc_cle __rkcore_MOD_clear_stat
#define rkc_print_error __rkcore_MOD_set_print_error
#define rkc_set_param  __rkcore_MOD_set_param
#define rkc_set_eunit  __rkcore_MOD_set_eunit
#define rkc_set_aunit  __rkcore_MOD_set_aunit
#define rkc_set_mass  __rkcore_MOD_set_mass
#define rkc_get_set_mass  __rkcore_MOD_get_set_mass
#define rkc_get_set_strip_mass  __rkcore_MOD_get_set_strip_mass
#define rkc_set_Ex  __rkcore_MOD_set_ex
#define rkc_set_K1  __rkcore_MOD_set_k1
#define rkc_set_K1c  __rkcore_MOD_set_k1c
#define rkc_set_p1  __rkcore_MOD_set_p1
#define rkc_set_p1c  __rkcore_MOD_set_p1c
#define rkc_set_K34c  __rkcore_MOD_set_k34c
#define rkc_set_th3Max  __rkcore_MOD_set_th3max
#define rkc_set_K3Sign __rkcore_MOD_set_k3sign
#define rkc_set_Theta3  __rkcore_MOD_set_theta3
#define rkc_set_Theta3c  __rkcore_MOD_set_theta3c
#define rkc_set_q  __rkcore_MOD_set_q

#define rkc_get_QValue __rkcore_MOD_get_qvalue
#define rkc_get_K1Min __rkcore_MOD_get_k1min
#define rkc_get_K1cMin __rkcore_MOD_get_k1cmin
#define rkc_get_p1Min __rkcore_MOD_get_p1min
#define rkc_get_p1cMin __rkcore_MOD_get_p1cmin

#define rkc_get_K1 __rkcore_MOD_get_k1
#define rkc_get_K3 __rkcore_MOD_get_k3
#define rkc_get_K4 __rkcore_MOD_get_k4

#define rkc_get_K1c __rkcore_MOD_get_k1c
#define rkc_get_K2c __rkcore_MOD_get_k1c
#define rkc_get_K3c __rkcore_MOD_get_k3c
#define rkc_get_K4c __rkcore_MOD_get_k4c

#define rkc_get_p1 __rkcore_MOD_get_p1
#define rkc_get_p3 __rkcore_MOD_get_p3
#define rkc_get_p4 __rkcore_MOD_get_p4

#define rkc_get_p1c __rkcore_MOD_get_p1c
#define rkc_get_p3c __rkcore_MOD_get_p3c
#define rkc_get_p4c __rkcore_MOD_get_p4c

#define rkc_get_E1 __rkcore_MOD_get_e1
#define rkc_get_E3 __rkcore_MOD_get_e3
#define rkc_get_E4 __rkcore_MOD_get_e4

#define rkc_get_E1c __rkcore_MOD_get_e1c
#define rkc_get_E2c __rkcore_MOD_get_e1c
#define rkc_get_E3c __rkcore_MOD_get_e3c
#define rkc_get_E4c __rkcore_MOD_get_e4c

#define rkc_get_gamma __rkcore_MOD_get_gamma
#define rkc_get_beta __rkcore_MOD_get_beta
#define rkc_get_betagamma __rkcore_MOD_get_betagamma
#define rkc_get_invgamma __rkcore_MOD_get_invgamma

#define rkc_get_qMax __rkcore_MOD_get_qmax
#define rkc_get_qMin __rkcore_MOD_get_qmin

#define rkc_get_ExMax __rkcore_MOD_get_exmax
#define rkc_get_Ex __rkcore_MOD_get_ex

#define rkc_get_th3 __rkcore_MOD_get_th3
#define rkc_get_th4 __rkcore_MOD_get_th4
#define rkc_get_th3c __rkcore_MOD_get_th3c
#define rkc_get_th4c __rkcore_MOD_get_th4c
#define rkc_get_th3Max __rkcore_MOD_get_th3max
#define rkc_get_th3cMax __rkcore_MOD_get_th3cmax
#define rkc_get_q __rkcore_MOD_get_q

#define rkc_get_J3 __rkcore_MOD_get_j3
#define rkc_get_J4 __rkcore_MOD_get_j4
#define rkc_get_KShift __rkcore_MOD_get_kshift
#define rkc_get_KFactor __rkcore_MOD_get_kfactor

#define rkc_get_invKin __rkcore_MOD_get_flag_invkin
#define rkc_get_th3MaxNe __rkcore_MOD_get_flag_th3maxne

#endif
