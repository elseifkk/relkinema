module rkcore
  use ISO_C_BINDING, only: C_SIZE_T, C_INT, C_BOOL
  use ISO_FORTRAN_ENV, only: ERROR_UNIT
  implicit none
  private

  integer,parameter::sp=selected_real_kind(6,37)
  integer,parameter::dp=selected_real_kind(15,307)
  integer,parameter::ep=selected_real_kind(18)
  integer,parameter::qp=selected_real_kind(33,4931)

#if !defined _NO_REAL16_
  integer,parameter::rp=qp
#elif !defined _NO_REAL10_
  integer,parameter::rp=ep
#else
  integer,parameter::rp=dp
#endif
  integer,parameter::outp=dp
  integer,parameter::inp=dp

  real(rp),parameter::reps=epsilon(1.0_rp)
  real(rp),parameter::oeps=epsilon(1.0_outp)
  real(rp),parameter::rzero=0.0_rp
  real(rp),parameter::pi=4.0_rp*atan(1.0_rp)
  real(rp),parameter::pi_2=pi/2.0_rp
  real(rp),parameter::hbarc=197.326968_rp

  integer,parameter::EU_KEV=1
  integer,parameter::EU_MEV=2
  integer,parameter::EU_GEV=3
  integer,parameter::EU_TEV=4
  real(rp),parameter::ENGU(4)=[&
       1.0e-3_rp,&
       1.0_rp,&
       1.0e3_rp,&
       1.0e6_rp]
  real(rp),parameter::ENGUR(4)=[&
       1.0e3_rp,&
       1.0_rp,&
       1.0e-3_rp,&
       1.0e-6_rp]
  real(rp),parameter::pirad=pi/180.0_rp

  integer,parameter::RKCST_SQRT_UNDER  = Z"0001"
  integer,parameter::RKCST_ASIN_UNDER  = Z"0002"
  integer,parameter::RKCST_ASIN_OVER   = Z"0004"
  integer,parameter::RKCST_ACOS_UNDER  = Z"0008"
  integer,parameter::RKCST_ACOS_OVER   = Z"0010"
  integer,parameter::RKCST_K_NEG       = Z"0020"
  integer,parameter::RKCST_TRUNC       = Z"0040"
  integer,parameter::nerrmess          = 7
  integer,parameter::len_errmess       = 32
  character(len_errmess),parameter::errmess(nerrmess)=[&
       "SQRT Underflow   ",&
       "ASIN Underflow   ",&
       "ASIN Overflow    ",&
       "ACOS Underflow   ",&
       "ACOS Overflow    ",&
       "Negative KE      ",&
       "Truncated to zero"&
       ]
  integer,parameter::RKCERR_RANGE_UNDER = 1
  integer,parameter::RKCERR_RANGE_OVER  = 2

  type t_rkc
     real(rp) amu_,me_
     real(rp) m10_,m20_,m30_,m40_
     real(rp) m1_,m2_,m3_,m4_
     real(rp) E1_,E2_,E3_,E4_
     real(rp) p1_,p2_,p3_,p4_
     real(rp) K1_,K2_,K3_,K4_
     real(rp) E1c_,E2c_,E3c_,E4c_
     real(rp) p1c_,p2c_,p3c_,p4c_
     real(rp) K1c_,K2c_,K3c_,K4c_
     real(rp) QValue_
     real(rp) Ex_, ExMax_
     real(rp) K1Min_, p1Min_
     real(rp) K1cMin_, p1cMin_
     real(rp) theM_
     real(rp) gamma_, beta_
     real(rp) th3Max_
     real(rp) th3_, th4_
     real(rp) theq_, qMin_, qMax_
     real(rp) th3c_, th4c_
     real(rp) J3_, J4_
     real(rp) KShift_, KFactor_
     logical  th3MaxNe, invKin, massSet, K3Sign
     integer stat
     integer eu
     logical deg
     logical elastic
     logical print_error
  end type t_rkc

  public init_rkc

#define amu rkc%amu_
#define me  rkc%me_

#define m1 rkc%m1_
#define m2 rkc%m2_
#define m3 rkc%m3_
#define m4 rkc%m4_

#define m10 rkc%m10_
#define m20 rkc%m20_
#define m30 rkc%m30_
#define m40 rkc%m40_

#define E1 rkc%E1_
#define E2 rkc%E2_
#define E3 rkc%E3_
#define E4 rkc%E4_
#define p1 rkc%p1_
#define p2 rkc%p2_
#define p3 rkc%p3_
#define p4 rkc%p4_
#define K1 rkc%K1_
#define K2 rkc%K2_
#define K3 rkc%K3_
#define K4 rkc%K4_
#define K1 rkc%K1_
#define K2 rkc%K2_
#define K3 rkc%K3_
#define K4 rkc%K4_

#define E1c rkc%E1c_
#define E2c rkc%E2c_
#define E3c rkc%E3c_
#define E4c rkc%E4c_
#define p1c rkc%p1c_
#define p2c rkc%p2c_
#define p3c rkc%p3c_
#define p4c rkc%p4c_
#define K1c rkc%K1c_
#define K2c rkc%K2c_
#define K3c rkc%K3c_
#define K4c rkc%K4c_
#define K1c rkc%K1c_
#define K2c rkc%K2c_
#define K3c rkc%K3c_
#define K4c rkc%K4c_

#define QValue rkc%QValue_
#define Ex rkc%Ex_
#define ExMax rkc%ExMax_
#define beta rkc%beta_
#define gamma rkc%gamma_
#define theM rkc%theM_
#define K1Min rkc%K1Min_
#define p1Min rkc%p1Min_
#define K1cMin rkc%K1cMin_
#define p1cMin rkc%p1cMin_

#define th3Max rkc%th3Max_
#define th3 rkc%th3_
#define th4 rkc%th4_
#define th3c rkc%th3c_
#define th4c rkc%th4c_
#define theq rkc%theq_
#define qMin rkc%qMin_
#define qMax rkc%qMax_
#define J3 rkc%J3_
#define J4 rkc%J4_
#define KShift rkc%KShift_
#define KFactor rkc%KFactor_

#define size_t integer(C_SIZE_T)
#define int integer(C_INT)
#define prkc_in(p) size_t,intent(in),value::p 
#define int_in(i) int,intent(in),value::i
#define prkc_inout(p) prkc_in(p); type(t_rkc) rkc; pointer(prkc,rkc); prkc=p
#define real_in(x) real(inp),intent(in),value::x
#define retint int
#define bool logical(C_BOOL)
#define retlog bool
#define bool_in(x) bool,intent(in),value::x

contains

  ! internal
  subroutine warn(rkc,fnc,code,x)
    type(t_rkc),intent(inout)::rkc
    character*(*),intent(in)::fnc
    integer,intent(in)::code
    real(rp),intent(in),optional::x
    rkc%stat=ior(rkc%stat,code)
    if(.not.rkc%print_error) return
    write(ERROR_UNIT,10) "*** Warning: "//trim(fnc)//": "//trim(get_errmess())
    if(present(x)) write(ERROR_UNIT,*) "Value=",x
10  format(x,a)

  contains

    character(len_errmess) function get_errmess()
      integer i,j
      get_errmess=""
      j=code
      do i=1,nerrmess
         j=ishft(j,-1)
         if(j==0) then
            get_errmess=errmess(i)
            return
         end if
      end do
    end function get_errmess

  end subroutine warn

  size_t function init_rkc()
    type(t_rkc) rkc
    pointer(prkc,rkc)
    init_rkc=malloc(sizeof(rkc))
    prkc=init_rkc
    call fillz
    rkc%stat=0
    rkc%eu=EU_MEV
    rkc%deg=.true.
    rkc%K3Sign=.false. ! positive 
    rkc%print_error=.true.
  contains
    subroutine fillz()
      integer i
      integer*1 c
      pointer(p,c)
      p=prkc-1
      do i=1,sizeof(rkc)
         p=p+1
         c=0
      end do
    end subroutine fillz
  end function init_rkc

  subroutine uinit_rkc(p)
    prkc_in(p)
    if(p/=0) call free(p)
  end subroutine uinit_rkc

  size_t function cp_rkc(p)
    prkc_in(p)
    type(t_rkc) rkc_in, rkc_out
    pointer(prkc_in,rkc_in)
    pointer(prkc_out,rkc_out)
    if(p==0) then
       cp_rkc=0
       return
    end if
    prkc_in=p
    cp_rkc=malloc(sizeof(rkc_in))
    prkc_out=cp_rkc
    rkc_out=rkc_in
  end function cp_rkc

  subroutine clear_stat(p)
    prkc_inout(p)
    rkc%stat=0
  end subroutine clear_stat

  subroutine set_print_error(p,b)
    bool_in(b)
    prkc_inout(p)
    rkc%print_error=b
  end subroutine set_print_error

  subroutine set_param(p,amu_,me_)
    real_in(amu_)
    real_in(me_)
    prkc_inout(p)
    amu=amu_
    me=me_
  end subroutine set_param

  subroutine set_K3Sign(p,s)
    int_in(s)
    prkc_inout(p)
    rkc%K3Sign=(s==-1)
  end subroutine set_K3Sign

  subroutine set_aunit(p,deg)
    int_in(deg)
    prkc_inout(p)
    rkc%deg=(deg/=0)    
  end subroutine set_aunit

  subroutine set_eunit(p,i)
    int_in(i)
    prkc_inout(p)
    if(i>0.and.i<=size(ENGU)) rkc%eu=i
  end subroutine set_eunit

  real(outp) function get_set_strip_mass(p,id,Z)
    real(rp) am
    int_in(id)
    int_in(Z)
    prkc_inout(p)
    select case(id)
    case(1)
       am=m10-real(Z,kind=rp)*me
       m1=am
    case(2)
       am=m20-real(Z,kind=rp)*me
       m2=am
    case(3)
       am=m30-real(Z,kind=rp)*me
       m3=am
    case(4)
       am=m40-real(Z,kind=rp)*me
       m4=am
    end select
    get_set_strip_mass=real(am,kind=outp)
  end function get_set_strip_mass

  ! internal
  logical function is_elastic(rkc)
    type(t_rkc),intent(in)::rkc
    is_elastic=(Ex==rzero.and.m1==m2.and.m2==m3.and.m3==m4)
  end function is_elastic

  real(outp) function get_set_mass(p,id,A,m)
    real(rp) am
    int_in(id)
    int_in(A)
    real_in(m)
    prkc_inout(p)
    if(A==0) then
       am=m
    else
       am=A*amu+m/1000.0_rp
    end if
    select case(id)
    case(1)
       m10=am
       m1=am
    case(2)
       m20=am
       m2=am
    case(3)
       m30=am
       m3=am
    case(4)
       m40=am
       m4=am
    end select
    get_set_mass=real(am,kind=outp)
  end function get_set_mass

  subroutine set_mass(p,m1_,m2_,m3_,m4_)
    real_in(m1_)
    real_in(m2_)
    real_in(m3_)
    real_in(m4_)
    prkc_inout(p)
    m1=m1_
    m2=m2_
    m3=m3_
    m4=m4_
    theM=real(m1,kind=rp)+real(m2,kind=rp)
    E2=m2
    rkc%massSet=.true.
    rkc%elastic=is_elastic(rkc)
  end subroutine set_mass

  ! internal
  real(rp) function K2p(K,m)
    real(rp),intent(in)::K,m
    K2p=sqrt(K*(K+2.0_rp*m))
  end function K2p

  ! internal
  real(rp) function p2E(p,m)
    real(rp),intent(in)::p,m
    p2E=sqrt(p*p+m*m)
  end function p2E

  subroutine set_Ex(p,Ex_)
    real(rp) m
    real_in(Ex_)
    prkc_inout(p)
    rkc%stat=0
    Ex=Ex_*ENGU(rkc%eu)
    rkc%elastic=is_elastic(rkc)
    QValue=(m1+m2)-(m3+(m4+Ex))
    if(QValue>rzero) then
       K1Min=rzero
       p1Min=rzero
    else
       K1Min=(-QValue)*(m1+m2+m3+(m4+Ex))/(2.0_rp*m2)
       p1Min=K2p(K1Min,m1)
    end if
    m=sqrt((m1+m2)**2.0_rp+2.0_rp*m2*K1Min)
    K1cMin=(m-m1+m2)*(m-(m1+m2))/(2.0_rp*m)
    p1cMin=K2p(K1cMin,m1)
  end subroutine set_Ex

  ! internal
  subroutine K1c_To_K1(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) m12,mm2,mm1
    m12=m1+m2
    mm1=m1*m1
    mm2=m2*m2
    theM=K1c+m1+sqrt(K1c*K1c+2.0_rp*m1*K1c+mm2)
    gamma=(theM*theM-mm1+mm2)/(2.0_rp*m2*theM)
    beta=sqrt(1.0_rp-1.0_rp/(gamma*gamma))
    K1=(theM-m12)*(theM+m12)/(2.0_rp*m2)
    E1=K1+m1
    p1=K2p(K1,m1)    
    ExMax=theM-(m3+m4)
  end subroutine K1c_To_K1

  ! internal
  subroutine K1_To_K1c(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) m12
    m12=m1+m2
    theM=sqrt(m12*m12+2.0_rp*m2*K1)
    gamma=(K1+m12)/sqrt(2.0_rp*K1*m2+m12*m12)
    beta=sqrt(K1*K1+2.0_rp*K1*m1)/(K1+m12)
    K1c=(theM-m1+m2)*(theM-m12)/(2.0_rp*theM)
    E1c=K1c+m1
    p1c=K2p(K1c,m1)
    ExMax=theM-(m3+m4)
  end subroutine K1_To_K1c

  subroutine set_K1(p,K1_)
    real_in(K1_)
    prkc_inout(p)
    rkc%stat=0
    K1=K1_*ENGU(rkc%eu)
    p1=K2p(K1,m1)
    E1=K1+m1
    call K1_To_K1c(rkc)
  end subroutine set_K1

  subroutine set_K1c(p,K1c_)
    real_in(K1c_)
    prkc_inout(p)
    rkc%stat=0
    K1c=K1c_*ENGU(rkc%eu)
    p1c=K2p(K1c,m1)
    E1c=K1c+m1
    call K1c_To_K1(rkc)
  end subroutine set_K1c

  subroutine set_p1(p,p1_)
    real_in(p1_)
    prkc_inout(p)
    rkc%stat=0
    p1=p1_*ENGU(rkc%eu)
    E1=p2E(p1,m1)
    K1=E1-m1
    call K1_To_K1c(rkc)
  end subroutine set_p1

  subroutine set_p1c(p,p1c_)
    real_in(p1c_)
    prkc_inout(p)
    rkc%stat=0
    p1c=p1c_*ENGU(rkc%eu)
    E1c=p2E(p1c,m1)
    K1c=E1c-m1
    call K1c_To_K1(rkc)
  end subroutine set_p1c
  
  ! internal
  subroutine set_K3_via_LB(rkc)
    type(t_rkc),intent(inout)::rkc
    E3=gamma*(E3c+beta*p3c*cos(th3c))
    K3=E3-m3
    p3=K2p(K3,m3)
  end subroutine set_K3_via_LB

  ! internal
  subroutine set_K3c(rkc)
    type(t_rkc),intent(inout)::rkc
    if(rkc%elastic) then
       E3c=sqrt((2.0_rp*m1*m1+m1*K1)/2.0_rp)
       p3c=sqrt(m1*K1/2.0_rp)
       K3c=E3c-m3
       return
    end if
    E3c=theM/2.0_rp+(m3+(m4+Ex))*(m3-(m4+Ex))/(2.0_rp*theM)
    K3c=E3c-m3
    if(K3c<rzero) then
       call warn(rkc,"set_K3c",RKCST_K_NEG,K3c)
       K3c=rzero
       E3c=m3
       p3c=rzero
    else
       p3c=K2p(K3c,m3)
    end if
  end subroutine set_K3c

  ! internal
  subroutine set_K4c(rkc)
    type(t_rkc),intent(inout)::rkc
    if(rkc%elastic) then
       K4c=K3c
       E4c=E3c
       p4c=p3c
       return
    end if
    E4c=theM-E3c
    K4c=E4c-(m4+Ex)
    if(K4c<rzero) then
       call warn(rkc,"set_K4c",RKCST_K_NEG,K4c)
       K4c=rzero
       E4c=(m4+Ex)
       p4c=rzero
    else
       p4c=K2p(K4c,m4+Ex)
    end if
  end subroutine set_K4c

  ! internal
  subroutine set_th3Max(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) b,d
    b=p3c-E3c*beta
    if(abs(b)<reps.or.rkc%elastic) then
       rkc%th3MaxNe=.true.
       rkc%invKin=.false.
       th3Max=pi_2
    else if(b<rzero) then
       rkc%th3MaxNe=.false.
       rkc%invKin=.true.
       d=p3c/(gamma*beta*m3)
       if(d>1.0_rp) then
          call warn(rkc,"set_th3Max",RKCST_ASIN_OVER,d)
          th3Max=pi_2
       else
          th3Max=asin(d)
       end if
    else
       rkc%th3MaxNe=.false.
       rkc%invKin=.false.
       th3Max=pi
    end if
  end subroutine set_th3Max

  ! internal
  subroutine set_qMinMax(rkc)
    type(t_rkc),intent(inout)::rkc
    qMin=abs(p1c-p3c)/hbarc
    qMax=(p1c+p3c)/hbarc
  end subroutine set_qMinMax

  subroutine set_K34c(p)
    prkc_inout(p)
    rkc%stat=0
    call set_K3c(rkc)
    call set_K4c(rkc)
    call set_th3Max(rkc)
    call set_qMinMax(rkc)
  end subroutine set_K34c

  ! internal
  subroutine set_K3(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) c,ss,d
    c=cos(th3)
    if(rkc%invKin) c=abs(c)
    ss=1.0_rp-(beta*c)**2.0_rp
    d=1.0_rp-(m3/E3c*gamma)**2.0_rp*ss
    if(d<rzero) then
       call warn(rkc,"set_K3",RKCST_SQRT_UNDER,d)
       d=rzero
    end if
    if(.not.rkc%K3Sign) then
       E3=E3c/gamma*(1.0_rp+beta*c*sqrt(d))/ss
    else
       E3=E3c/gamma*(1.0_rp-beta*c*sqrt(d))/ss
    end if
    K3=E3-m3
    if(K3<rzero) then
       call warn(rkc,"set_K3",RKCST_K_NEG,K3)
       K3=rzero
       E3=m3
       p3=rzero
    else
       p3=K2p(K3,m3)
    end if
  end subroutine set_K3

  ! internal
  subroutine set_K4(rkc)
    type(t_rkc),intent(inout)::rkc
    E4=(E1+E2)-E3
    K4=E4-(m4+Ex)
    if(K4<rzero) then
       call warn(rkc,"set_K4",RKCST_K_NEG,K4)
       K4=rzero
       E4=m4+Ex
       p4=rzero
    else
       p4=K2p(K4,m4+Ex)
    end if
  end subroutine set_K4

  ! internal
  subroutine th3_To_th4(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) c
    if(p4==rzero) then
       th4=pi_2
       return
    else if(p3==rzero) then
       th4=rzero
       return
    else if(th3==rzero.or.abs(pi-th3)<=2.0_rp*reps) then
       if(.not.rkc%th3MaxNe) then
          th4=rzero
       else
          th4=pi_2 ! p4 should be zero...
       end if
       return
    end if
    c=(p1*p1-(p3*p3+p4*p4))/(2.0_rp*p3*p4)
    if(c>1.0_rp) then
       call warn(rkc,"th3_To_th4",RKCST_ACOS_OVER,c)
       th4=rzero
    else if(c<-1.0_rp) then
       call warn(rkc,"th3_To_th4",RKCST_ACOS_UNDER,c)
       th4=pi-th3
    else
       th4=acos(c)-th3
    end if
  end subroutine th3_To_th4

  ! internal
  subroutine th3_To_th3c(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) c
    if(th3==rzero) then
       if(rkc%invKin.and.rkc%K3Sign) then
          th3c=pi
       else
          th3c=rzero
       end if
       return
    else if(abs(pi-th3)<=2.0_rp*reps) then
       th3c=pi
       return
    end if
    c=gamma*(p3*cos(th3)-beta*E3)/p3c
    if(c>1.0_rp) then
       call warn(rkc,"th3_To_th3c",RKCST_ACOS_OVER,c)
       th3c=rzero
    else if(c<-1.0_rp) then
       call warn(rkc,"th3_To_th3c",RKCST_ACOS_UNDER,c)
       th3c=pi
    else
       th3c=acos(c)
    end if
  end subroutine th3_To_th3c

  ! internal
  subroutine th3c_To_th3(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) c
    c=gamma*(p3c*cos(th3c)+beta*E3c)/p3
    if(c>1.0_rp) then
       call warn(rkc,"th3c_To_th3",RKCST_ACOS_OVER,c)
       th3=rzero
    else if(c<-1.0_rp) then
       call warn(rkc,"th3c_To_th3",RKCST_ACOS_UNDER,c)
       th3=pi
    else
       th3=acos(c)
    end if
  end subroutine th3c_To_th3

  ! internal
  subroutine th3c_To_th4c(rkc)
    type(t_rkc),intent(inout)::rkc
    th4c=pi-th3c
  end subroutine th3c_To_th4c

  ! internal
  real(rp) function tcm2q(rkc,tcm,p_i,p_f)
    type(t_rkc),intent(inout)::rkc
    real(rp),intent(in)::tcm,p_i,p_f
    real(rp) q
    q=p_i*p_i+p_f*p_f-2.0*p_i*p_f*cos(tcm)
    if(q<rzero) then
       call warn(rkc,"tcm2q",RKCST_SQRT_UNDER,q)
       tcm2q=rzero
    else
       tcm2q=sqrt(q)/hbarc
    end if
  end function tcm2q

  ! internal
  real(rp) function q2tcm(rkc,q,p_i,p_f)
    type(t_rkc),intent(inout)::rkc
    real(rp),intent(in)::q,p_i,p_f
    real(rp) c
    c=(p_i*p_i+p_f*p_f-(q*hbarc)**2.0_rp)/(2.0_rp*p_i*p_f)
    if(c>1.0_rp) then
       call warn(rkc,"th3c_To_th3",RKCST_ACOS_OVER,c)
       q2tcm=rzero
    else if(c<-1.0_rp) then
       call warn(rkc,"th3c_To_th3",RKCST_ACOS_UNDER,c)
       q2tcm=pi
    else
       q2tcm=acos(c)
    end if
  end function q2tcm

  retint function set_Theta3(p,th3_)
    real_in(th3_)
    prkc_inout(p)
    rkc%stat=0
    th3=th3_
    if(rkc%deg) th3=th3*pirad
    if(th3<rzero) then
       set_Theta3=RKCERR_RANGE_UNDER
       th3=rzero
    else if(th3>th3Max) then
       set_Theta3=RKCERR_RANGE_OVER
       th3=th3Max
    else
       set_Theta3=0
    end if
    call set_K3(rkc)
    call set_K4(rkc)
    call th3_To_th4(rkc)
    call th3_To_th3c(rkc)
    call th3c_To_th4c(rkc)
    theq=tcm2q(rkc,th3c,p1c,p3c)
    call set_other_params(rkc)
  end function set_Theta3

  retint function set_Theta3c(p,th3c_)
    real_in(th3c_)
    prkc_inout(p)
    rkc%stat=0
    th3c=th3c_
    if(rkc%deg) th3c=th3c*pirad
    if(th3c<rzero) then
       set_Theta3c=RKCERR_RANGE_UNDER
       th3c=rzero
    else if(th3c>pi) then
       set_Theta3c=RKCERR_RANGE_OVER
       th3c=pi
    else
       set_Theta3c=0
    end if
    call set_K3_via_LB(rkc)
    call set_K4(rkc)
    call th3c_To_th3(rkc)
    call th3_To_th4(rkc)
    call th3c_To_th4c(rkc)
    theq=tcm2q(rkc,th3c,p1c,p3c)
    call set_other_params(rkc)
  end function set_Theta3c

  integer function set_q(p,q_)
    real_in(q_)
    prkc_inout(p)
    rkc%stat=0
    theq=q_
    if(theq<qMin) then
       set_q=RKCERR_RANGE_UNDER
       theq=qMin
    else if(theq>qMax) then
       set_q=RKCERR_RANGE_OVER
       theq=qMax
    else
       set_q=0
    end if
    th3c=q2tcm(rkc,theq,p1c,p3c)
    call set_K3_via_LB(rkc)
    call set_K4(rkc)
    call th3c_To_th4c(rkc)
    call th3c_To_th3(rkc)
    call th3_To_th4(rkc)
    call set_other_params(rkc)
  end function set_q

  real(outp) function get_QValue(p)
    prkc_inout(p)
    get_QValue=real(QValue*ENGUR(rkc%eu),kind=outp)
  end function get_QValue

  !
  real(outp) function get_K1Min(p)
    prkc_inout(p)
    get_K1Min=real(K1Min*ENGUR(rkc%eu),kind=outp)
  end function get_K1Min

  real(outp) function get_K1cMin(p)
    prkc_inout(p)
    get_K1cMin=real(K1cMin*ENGUR(rkc%eu),kind=outp)
  end function get_K1cMin

  real(outp) function get_p1Min(p)
    prkc_inout(p)
    get_p1Min=real(p1Min*ENGUR(rkc%eu),kind=outp)
  end function get_p1Min

  real(outp) function get_p1cMin(p)
    prkc_inout(p)
    get_p1cMin=real(p1cMin*ENGUR(rkc%eu),kind=outp)
  end function get_p1cMin

  !
  real(outp) function get_K1(p)
    prkc_inout(p)
    get_K1=real(K1*ENGUR(rkc%eu),kind=outp)
  end function get_K1

  real(outp) function get_K3(p)
    prkc_inout(p)
    get_K3=real(K3*ENGUR(rkc%eu),kind=outp)
  end function get_K3

  real(outp) function get_K4(p)
    prkc_inout(p)
    get_K4=real(K4*ENGUR(rkc%eu),kind=outp)
  end function get_K4

  !
  real(outp) function get_p1(p)
    prkc_inout(p)
    get_p1=real(p1*ENGUR(rkc%eu),kind=outp)
  end function get_p1

  real(outp) function get_p3(p)
    prkc_inout(p)
    get_p3=real(p3*ENGUR(rkc%eu),kind=outp)
  end function get_p3

  real(outp) function get_p4(p)
    prkc_inout(p)
    get_p4=real(p4*ENGUR(rkc%eu),kind=outp)
  end function get_p4

  !
  real(outp) function get_p1c(p)
    prkc_inout(p)
    get_p1c=real(p1c*ENGUR(rkc%eu),kind=outp)
  end function get_p1c

  real(outp) function get_p3c(p)
    prkc_inout(p)
    get_p3c=real(p3c*ENGUR(rkc%eu),kind=outp)
  end function get_p3c

  real(outp) function get_p4c(p)
    prkc_inout(p)
    get_p4c=real(p4c*ENGUR(rkc%eu),kind=outp)
  end function get_p4c

  !
  real(outp) function get_K1c(p)
    prkc_inout(p)
    get_K1c=real(K1c*ENGUR(rkc%eu),kind=outp)
  end function get_K1c

  real(outp) function get_K2c(p)
    prkc_inout(p)
    get_K2c=real(K2c*ENGUR(rkc%eu),kind=outp)
  end function get_K2c

  real(outp) function get_K3c(p)
    prkc_inout(p)
    get_K3c=real(K3c*ENGUR(rkc%eu),kind=outp)
  end function get_K3c

  real(outp) function get_K4c(p)
    prkc_inout(p)
    get_K4c=real(K4c*ENGUR(rkc%eu),kind=outp)
  end function get_K4c

  !
  real(outp) function get_E1(p)
    prkc_inout(p)
    get_E1=real(E1*ENGUR(rkc%eu),kind=outp)
  end function get_E1

  real(outp) function get_E3(p)
    prkc_inout(p)
    get_E3=real(E3*ENGUR(rkc%eu),kind=outp)
  end function get_E3

  real(outp) function get_E4(p)
    prkc_inout(p)
    get_E4=real(E4*ENGUR(rkc%eu),kind=outp)
  end function get_E4

  !
  real(outp) function get_E1c(p)
    prkc_inout(p)
    get_E1c=real(E1c*ENGUR(rkc%eu),kind=outp)
  end function get_E1c

  real(outp) function get_E2c(p)
    prkc_inout(p)
    get_E2c=real(E2c*ENGUR(rkc%eu),kind=outp)
  end function get_E2c

  real(outp) function get_E3c(p)
    prkc_inout(p)
    get_E3c=real(E3c*ENGUR(rkc%eu),kind=outp)
  end function get_E3c

  real(outp) function get_E4c(p)
    prkc_inout(p)
    get_E4c=real(E4c*ENGUR(rkc%eu),kind=outp)
  end function get_E4c

  !
  real(outp) function get_gamma(p)
    prkc_inout(p)
    get_gamma=real(gamma,kind=outp)
  end function get_gamma

  real(outp) function get_beta(p)
    prkc_inout(p)
    get_beta=real(beta,kind=outp)
  end function get_beta

  real(outp) function get_betagamma(p)
    prkc_inout(p)
    get_betagamma=real(beta*gamma,kind=outp)
  end function get_betagamma

  real(outp) function get_invgamma(p)
    prkc_inout(p)
    get_invgamma=real(1.0_rp/gamma,kind=outp)
  end function get_invgamma

  !
  real(outp) function get_qMax(p)
    prkc_inout(p)
    get_qMax=real(qMax,kind=outp)
  end function get_qMax

  real(outp) function get_qMin(p)
    prkc_inout(p)
    get_qMin=real(qMin,kind=outp)
  end function get_qMin

  real(outp) function get_q(p)
    prkc_inout(p)
    get_q=real(theq,kind=outp)
  end function get_q

  !
  real(outp) function get_ExMax(p)
    prkc_inout(p)
    get_ExMax=real(ExMax*ENGUR(rkc%eu),kind=outp)
  end function get_ExMax

  real(outp) function get_Ex(p)
    prkc_inout(p)
    get_Ex=real(Ex*ENGUR(rkc%eu),kind=outp)
  end function get_Ex

  !
  real(outp) function get_th3Max(p)
    real(rp) x
    prkc_inout(p)
    x=th3Max
    if(x<oeps.and.x/=rzero) then
       call warn(rkc,"get_th3Max",RKCST_TRUNC,x)
       x=rzero
    end if
    if(rkc%deg) x=x/pirad
    get_th3Max=real(x,kind=outp)
  end function get_th3Max

  real(outp) function get_th3cMax(p)
    real(rp) x
    prkc_inout(p)
    x=pi
    if(rkc%deg) x=x/pirad
    get_th3cMax=real(x,kind=outp)
  end function get_th3cMax

  real(outp) function get_th3(p)
    real(rp) x
    prkc_inout(p)
    x=th3
    if(x<oeps.and.x/=rzero) then
       call warn(rkc,"get_th3",RKCST_TRUNC,x)
       x=rzero
    end if
    if(rkc%deg) x=x/pirad
    get_th3=real(x,kind=outp)
  end function get_th3

  real(outp) function get_th3c(p)
    real(rp) x
    prkc_inout(p)
    x=th3c
    if(x<oeps.and.x/=rzero) then
       call warn(rkc,"get_th3c",RKCST_TRUNC,x)
       x=rzero
    end if
    if(rkc%deg) x=x/pirad
    get_th3c=real(x,kind=outp)
  end function get_th3c

  !
  real(outp) function get_th4(p)
    real(rp) x
    prkc_inout(p)
    x=th4
    if(x<oeps.and.x/=rzero) then
       call warn(rkc,"get_th4",RKCST_TRUNC,x)
       x=rzero
    end if
    if(rkc%deg) x=x/pirad
    get_th4=real(x,kind=outp)
  end function get_th4

  real(outp) function get_th4c(p)
    real(rp) x
    prkc_inout(p)
    x=th4c
    if(x<oeps.and.x/=rzero) then
       call warn(rkc,"get_th4c",RKCST_TRUNC,x)
       x=rzero
    end if
    if(rkc%deg) x=x/pirad
    get_th4c=real(x,kind=outp)
  end function get_th4c

  ! internal
  subroutine set_other_params(rkc)
    type(t_rkc),intent(inout)::rkc
    real(rp) c
    c=cos(th3c)
    J3=(gamma*p3*(p3c+beta*E3c*c))/(p3c*p3c)
    J4=(gamma*p4*(p4c-beta*E4c*c))/(p4c*p4c)
    KShift=sin(th3)*J3*gamma*p3c*beta
    KFactor=KShift*E3/(p3*p3)
  end subroutine set_other_params

  !
  real(outp) function get_J3(p)
    prkc_inout(p)
    get_J3=real(J3,kind=outp)
  end function get_J3

  real(outp) function get_J4(p)
    prkc_inout(p)
    get_J4=real(J4,kind=outp)
  end function get_J4

  !
  real(outp) function get_KShift(p)
    real(rp) x
    prkc_inout(p)
    x=KShift
    if(rkc%deg) x=x*pirad
    get_KShift=real(x,kind=outp)
  end function get_KShift

  real(outp) function get_KFactor(p)
    real(rp) x
    prkc_inout(p)
    x=KFactor
    if(rkc%deg) x=x*pirad
    get_KFactor=real(x,kind=outp)
  end function get_KFactor

  !
  retlog function get_flag_invKin(p)
    prkc_inout(p)
    get_flag_invKin=rkc%invKin
  end function get_flag_invKin

  retlog function get_flag_th3MaxNe(p)
    prkc_inout(p)
    get_flag_th3MaxNe=rkc%th3MaxNe
  end function get_flag_th3MaxNe

end module rkcore