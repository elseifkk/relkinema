module plist
implicit none

type t_plist
   integer sz ! total size
   integer st ! stack top
   integer n  ! num elements
   integer p  ! str list
   integer nmax ! size of v(:)
   real*8,allocatable:: v(:)
end type t_plist

integer,parameter::PLERR_NOMEM = 1
integer,parameter::PLERR_MEMOV = 2
integer,parameter::PLERR_NOENT = 3

integer,parameter::LEN_PLIST_HDR=2 ! len+code

integer*1,parameter::PC_VAL = 0
integer*1,parameter::PC_REF = 1

contains

  function init_plist(sz,nmax)
    type(t_plist) init_plist
    integer,intent(in)::sz
    integer,intent(in)::nmax
    init_plist%p=malloc(sz)
    init_plist%sz=sz
    init_plist%st=init_plist%p-1
    init_plist%n=0
    init_plist%nmax=nmax
    allocate(init_plist%v(nmax))
  end function init_plist
  
  subroutine dump_plist(pl)
    type(t_plist),intent(in)::pl
    integer i,len,code
    character*256 s
    integer*1 b
    integer si
    real*8 v
    pointer(rp,v)
    pointer(p,b)
    pointer(ptr,si)
    p=pl%p-1
    do i=1,pl%n
       p=p+1
       len=b
       p=p+1
       code=b
       call cpstr()
       write(*,10) i,code,s(1:len)
       select case(code)
       case(PC_VAL)
          write(*,*) pl%v(i)
       case(PC_REF)
          ptr=loc(pl%v(i))
          rp=si
          write(*,20) si
          write(*,*) v
       end select
    end do
20  format(x,z16,$)
10  format(x,i4,x,i3,x,a,$)
  contains
    subroutine cpstr()
      integer j
      do j=1,len
         p=p+1
         s(j:j)=achar(b)
      end do
    end subroutine cpstr
  end subroutine dump_plist

  integer function find_par(pl,s,v)
    type(t_plist),intent(in)::pl
    character*(*),intent(in)::s
    real*8,intent(out)::v
    integer k
    integer*1 c
    integer si
    real*8 x
    pointer(ptr,si)
    pointer(pr,x)
    k=find_str(pl,s,c)
    if(k==0) then
       find_par=PLERR_NOENT
       return
    end if
    select case(c)
    case(PC_VAL)
       v=pl%v(k)
    case(PC_REF)
       ptr=loc(pl%v(k))
       pr=si
       v=x
    end select
    find_par=0
  end function find_par

  integer function change_pc(pl,k,code)
    type(t_plist),intent(in)::pl
    integer,intent(in)::k
    integer*1,intent(in)::code
    integer*1 b
    integer len
    integer i
    pointer(p,b)
    if(pl%n<k.or.k<=0) then
       change_pc=PLERR_NOENT
       return
    end if
    p=pl%p
    do i=1,k-1
       len=b
       p=p+len+LEN_PLIST_HDR
    end do
    p=p+1
    b=code
  end function change_pc

  integer function try_add_par(pl,s,code,k)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer*1,intent(in)::code
    integer,intent(out)::k
    integer istat
    integer*1 c
    if(pl%n>0) then
       k=find_str(pl,s,c)
       if(k/=0.and.c/=code) then
          istat=change_pc(pl,k,code)
          if(istat/=0) try_add_par=istat
       end if
    else
       k=0
    end if
    if(k==0) then
       if(pl%n>=pl%nmax) then
          try_add_par=PLERR_MEMOV
          return
       end if
       istat=add_str(pl,s,code)
       if(istat/=0) then
          try_add_par=istat
          return
       end if
       k=pl%n
    end if
    try_add_par=0
  end function try_add_par

  integer function add_par_by_reference(pl,s,ptr)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer,intent(in)::ptr
    integer istat
    integer k
    integer si !<<<<<<<<< size undetermined
    pointer(p,si)
    istat=try_add_par(pl,s,PC_REF,k)
    if(istat/=0) then
       add_par_by_reference=istat
       return
    end if
    pl%v(k)=0
    p=loc(pl%v(k))
    si=ptr
    add_par_by_reference=0
  end function add_par_by_reference

  integer function add_par_by_value(pl,s,v)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    real*8,intent(in)::v
    integer istat
    integer k
    istat=try_add_par(pl,s,PC_VAL,k)
    if(istat/=0) then
       add_par_by_value=istat
       return
    end if
    pl%v(k)=v
    add_par_by_value=0
  end function add_par_by_value

  integer function add_str(pl,s,code)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer*1,intent(in)::code
    integer i
    integer*1 b
    pointer(p,b)
    integer len
    if(pl%p==0) then
       add_str=PLERR_NOMEM
       return
    end if
    len=len_trim(s)
    if(len+LEN_PLIST_HDR+pl%st>pl%sz+pl%p) then
       add_str=PLERR_MEMOV
       return ! realloc needed
    end if
    if(len>255) then
       add_str=PLERR_MEMOV
       return
    end if
    p=pl%st+1
    b=int(len,kind=1)
    p=p+1
    b=code
    do i=1,len
       p=p+1
       b=ichar(s(i:i))
    end do
    pl%st=p
    pl%n=pl%n+1 
    add_str=0
  end function add_str

  integer function find_str(pl,s,code)
    type(t_plist),intent(in)::pl
    character*(*),intent(in)::s
    integer*1,intent(out)::code
    integer i
    integer len
    integer lenp
    integer*1 b
    pointer(p,b)
    find_str=0
    p=pl%p
    len=len_trim(s)
    do i=1,pl%n
       lenp=b
       if(len==lenp) then
          if(is_matched()) then
             find_str=i
             return
          end if
       end if
       p=p+lenp+LEN_PLIST_HDR
    end do
  contains
    logical function is_matched()
      integer*1 c
      integer j
      pointer(q,c)
      is_matched=.false.
      q=p+1 ! skip code
      code=c
      do j=1,len
         q=q+1
         if(c/=ichar(s(j:j))) return
      end do
      is_matched=.true.
    end function is_matched
  end function find_str

end module plist

module rpn
  use plist
  implicit none

#define _EXPR_(i) expr(rpnq(i)%p1:rpnq(i)%p2)
  private
  type(t_plist) pars
  character*16384 expr
  integer len_expr
  integer cur_pos
  integer old_pos
  integer old_tid

  integer,parameter::RPNERR_NOENT  = 1
  integer,parameter::RPNERR_NOOP   = 2
  integer,parameter::RPNERR_NOPAR  = 3
  integer,parameter::RPNERR_NOFNC  = 4
  integer,parameter::RPNERR_ADDPAR = 5
  integer,parameter::RPNERR_INVASN = 6
  integer,parameter::RPNERR_INVOP  = 7
  integer,parameter::RPNERR_INVFNC = 8

  ! meta tid
  integer,parameter::TID_FIN   = -999
  integer,parameter::TID_UNDEF =  0
  integer,parameter::TID_INV   =  -666
  ! operators
  integer,parameter::TID_ASN   =  1  ! =
  integer,parameter::TID_BOP1  =  2  ! +,-
  integer,parameter::TID_BOP1U=  -2
  integer,parameter::TID_BOP2  =  3  ! *,/
  integer,parameter::TID_BOP2U = -3  ! *,/
  integer,parameter::TID_UOP1  =  4  ! !
  integer,parameter::TID_BOP4  =  5  ! implicit * <<<<<<<<<<< 
  integer,parameter::TID_BOP3  =  6  ! ^,**,e
  integer,parameter::TID_UOP2  =  7  ! +,-
  integer,parameter::TID_FNC   =  8  ! sin, cos,...
  ! braket
  integer,parameter::TID_BRA   = -9   ! (
  integer,parameter::TID_KET   = -10  ! )
  ! variables
  integer,parameter::TID_PARU  = -11  ! a,b,c,...
  integer,parameter::TID_PAR   =  11  ! a,b,c,...
  integer,parameter::TID_FIG   = 12  ! 1,2,3,...
  integer,parameter::TID_VAR   = 13  ! fig in rbuf
  ! do nothing or undef
  integer,parameter::TID_NOP   = 14

  type t_rpnq
     integer tid
     integer p1
     integer p2
  end type t_rpnq

  type(t_rpnq),allocatable::rpnq(:)
  type(t_rpnq),allocatable::buf(:)
  real*8,allocatable::rbuf(:)
  integer p_buf,p_rpnq,p_rbuf

  real*8::answer=2.99792458e8

  public init_rpn
  public init_par
  public build_rpnq
  public rpn_eval
  public rpn_ans
  public dump_rpnq
  public print_pars

contains

  real*8 function rpn_ans()
    rpn_ans=answer
  end function rpn_ans
  
  subroutine init_rpn(s)
    character*(*),intent(in)::s
    expr=trim(adjustl(s))
    len_expr=strip(expr)
    cur_pos=0
    old_pos=0
    old_tid=0
    p_buf=0
    p_rpnq=0
    p_rbuf=0
    if(allocated(rpnq)) deallocate(rpnq)
    if(allocated(buf)) deallocate(buf)
    if(allocated(rbuf)) deallocate(rbuf)
    allocate(rpnq(len_expr*2)) ! << at most
    allocate(buf(len_expr*2)) ! << at most
    allocate(rbuf(len_expr)) ! must be calculated!
  end subroutine init_rpn
  
  integer function strip(s)
    character*(*),intent(inout)::s
    integer i,k
    k=0
    do i=1,len_trim(s)
       select case(s(i:i))
       case(" ","\t")
       case default
          k=k+1
          if(k/=i) s(k:k)=s(i:i)
       end select
    end do
    strip=k
  end function strip

  subroutine init_par()
    integer istat
    pars=init_plist(4096,128) !<<<<<<<<<<<<<<
    istat=add_par_by_reference(pars,"ans",loc(answer))
    istat=add_par_by_value(pars,"pi",atan(1.)*4.)
    call print_pars()
  end subroutine init_par

  subroutine print_pars()
    call dump_plist(pars)
  end subroutine print_pars

  integer function get_operand(i)
    integer,intent(in)::i
    integer j
    get_operand=0
    do j=i,1,-1
       select case(rpnq(j)%tid)
       case(TID_FIG,TID_PAR,TID_VAR)
          get_operand=j
          return
       end select
    end do
  end function get_operand

  integer function get_par_value(i,v)
    integer,intent(in)::i
    real*8,intent(out)::v
    integer istat
    istat=find_par(pars,_EXPR_(i),v)
    if(istat/=0) then
       write(*,*) "*** No such parameter: "//_EXPR_(i)
       get_par_value=RPNERR_NOENT
    else
       get_par_value=0
    end if
  end function get_par_value

  integer function get_value(i,v)
    integer,intent(in)::i
    real*8,intent(out)::v
    integer istat
    select case(rpnq(i)%tid)
    case(TID_FIG)
       read(_EXPR_(i),*,iostat=istat) v
    case(TID_VAR)         
       if(rpnq(i)%p1>p_rbuf) then
          istat=RPNERR_NOENT
       else
          v=rbuf(rpnq(i)%p1)
          istat=0
       end if
    case(TID_PAR)
       istat=get_par_value(i,v)
    case default
       stop "*** UNEXPECTED ERROR in get_value"
    end select
    get_value=istat
  end function get_value

  real*8 function fac(v)
    real*8,intent(in)::v
    fac=v ! <<<
  end function fac

  integer function eval_1(i)
    integer,intent(in)::i
    integer od,istat
    real*8 v
    od=get_operand(i-1)
    if(od<=0) then
       eval_1=RPNERR_NOOP
       return
    end if
    istat=get_value(od,v)
    if(istat/=0) then
       eval_1=istat
       return
    end if
    select case(_EXPR_(i))
    case("+")
    case("-")
       v=-v
    case("!")
       v=fac(v)
    case default
       write(*,*) "*** Invalid operator: "//_EXPR_(i)
       eval_1=RPNERR_INVOP
       return
    end select
    call put_value(i,v)
    rpnq(od)%tid=TID_NOP
    eval_1=0
    answer=v
  end function eval_1

  subroutine put_value(i,v)
    integer,intent(in)::i
    real*8,intent(in)::v
    p_rbuf=p_rbuf+1
    rpnq(i)%tid=TID_VAR
    rpnq(i)%p1=p_rbuf
    rbuf(p_rbuf)=v
  end subroutine put_value

  integer function eval_2a(i)
    integer,intent(in)::i
    integer od1,od2
    integer istat
    real*8 v
    od2=get_operand(i-1)
    if(od2<=1) then
       eval_2a=RPNERR_NOOP
       return
    end if
    od1=get_operand(od2-1)
    if(od1<=0) then
       eval_2a=RPNERR_NOOP
       return
    end if
    if(rpnq(od1)%tid/=TID_PAR) then
       eval_2a=RPNERR_INVASN
       write(*,*) "*** Invalid asignment to non-parameter"
       return
    end if
    istat=get_value(od2,v)
    if(istat/=0) then
       eval_2a=istat
       return
    end if
    istat=add_par_by_value(pars,_EXPR_(od1),v)
    if(istat/=0) then
       eval_2a=RPNERR_ADDPAR
    else
       rpnq(od2)%tid=TID_NOP
       rpnq(i)%tid=TID_NOP
       eval_2a=0
       answer=v
    end if
  end function eval_2a

  integer function eval_2(i)
    integer,intent(in)::i
    character*4 op
    integer od1,od2
    real*8 v1,v2
    integer istat

    od2=get_operand(i-1)
    if(od2<=1) then
       eval_2=RPNERR_NOOP
       return
    end if
    od1=get_operand(od2-1)
    if(od1<=0) then
       eval_2=RPNERR_NOOP
       return
    end if

    istat=get_value(od1,v1)
    if(istat/=0) then
       eval_2=istat
       return
    end if
    istat=get_value(od2,v2)
    if(istat/=0) then
       eval_2=istat
       return
    end if

    if(rpnq(i)%p1==0) then
       op="*"
    else
       op=_EXPR_(i)
    end if
    select case(op)
    case("+")
       v1=v1+v2
    case("-")
       v1=v1-v2
    case("*")
       v1=v1*v2
    case("/")
       v1=v1/v2
    case("**","^")
       v1=v1**v2
    case("e")
       v1=v1*10**v2
    case default
       write(*,*) "*** Invalid operator: "//_EXPR_(i)
       eval_2=RPNERR_INVOP
       return
    end select
    call put_value(i,v1)
    rpnq(od1)%tid=TID_NOP
    rpnq(od2)%tid=TID_NOP
    eval_2=0
    answer=v1
  end function eval_2

  integer function eval_f1(i)
    integer,intent(in)::i
    integer od,istat
    real*8 v
    od=get_operand(i-1)
    if(od<=0) then
       eval_f1=RPNERR_NOOP
       return
    end if
    istat=get_value(od,v)
    if(istat/=0) then
       eval_f1=istat
       return
    end if
    select case(_EXPR_(i))
    case("sin")
       v=sin(v)
    case("cos")
       v=cos(v)
    case("tan")
       v=tan(v)
    case("asin")
       v=atan(v)
    case("acos")
       v=acos(v)
    case("atan")
       v=atan(v)
    case("exp")
       v=exp(v)
    case("sqrt")
       v=sqrt(v)
    case("log")
       v=log(v)
    case("log10")
       v=log10(v)
    case("sinh")
       v=sinh(v)
    case("cosh")
       v=cosh(v)
    case("tanh")
       v=tanh(v)
    case("asinh")
       v=log(v+sqrt(v*v+1))
    case("acosh")
       v=log(v+sqrt(v*v-1))
    case("atanh")
       v=log((v+1)/(v-1))/2.
    case default
       write(*,*) "*** Invalid function: "//_EXPR_(i)
       eval_f1=RPNERR_INVFNC
       return
    end select
    call put_value(i,v)
    rpnq(od)%tid=TID_NOP
    answer=v
    eval_f1=0
  end function eval_f1

  integer function rpn_eval()
    integer i
    integer istat
    integer ec
    real*8 v
    istat=0
    ec=0
    do i=1,p_rpnq
       ec=ec+1
       select case(rpnq(i)%tid)
       case(TID_UOP1,TID_UOP2)
          istat=eval_1(i)
       case(TID_BOP1,TID_BOP2,TID_BOP3,TID_BOP4)
          istat=eval_2(i)
       case(TID_FNC)
          istat=eval_f1(i)
       case(TID_ASN)
          istat=eval_2a(i)
       case default
          ec=ec-1
       end select
       if(istat/=0) exit
    end do
    if(ec==0.and.p_rpnq==1) then ! only fig or par
       istat=get_value(1,v)
       if(istat==0) answer=v
    end if
    rpn_eval=istat
  end function rpn_eval

  integer function get_end_of_fig(k_)
    integer,intent(in)::k_
    integer c
    integer a
    integer k
    
    k=k_
    if(expr(k:k)==".") then
       c=1
    else
       c=0
    end if
    
    do
       if(k>=len_expr) then
          get_end_of_fig=k
          return
       end if
       k=k+1
       a=ichar(expr(k:k))
       if(.not.is_numeric(a)) then
          if(c==1.and.k-1==k_) then
             get_end_of_fig=-k_ ! only .
          else
             get_end_of_fig=k-1
          end if
          return
       else if(expr(k:k)==".") then
          c=c+1
          if(c>1) then
             get_end_of_fig=-k ! second . found 
             return
          end if
       end if
    end do
    
  end function get_end_of_fig
  
  integer function get_end_of_par(k_)
    integer,intent(in)::k_
    integer k
    integer a
    k=k_
    do
       if(k>=len_expr) then
          get_end_of_par=k
          return
       end if
       k=k+1
       a=ichar(expr(k:k))
       if(.not.is_alpha(a).and..not.is_number(a)) then
          get_end_of_par=k-1
          return
       end if
    end do
  end function get_end_of_par
  
  logical function is_alpha(a)
    integer,intent(in)::a
    integer b
    b=ior(a,32)
    is_alpha=(b>=97.and.b<=122)
  end function is_alpha
  
  logical function is_number(a)
    integer,intent(in)::a
    is_number=(a>=48.and.a<=57)
  end function is_number
  
  logical function is_numeric(a)
    integer,intent(in)::a
    is_numeric=(is_number(a).or.a==46)
  end function is_numeric
  
  integer function get_tid(a)
    character*1,intent(in)::a
    get_tid=0 ! in order to avoid warning
    select case(a)
    case("+","-")
       get_tid=TID_BOP1U
    case("*")
       get_tid=TID_BOP2U
    case("/")
       get_tid=TID_BOP2
    case("^")
       get_tid=TID_BOP3
    case("(")
       get_tid=TID_BRA
    case(")")
       get_tid=TID_KET
    case("=")
       get_tid=TID_ASN
    case default
       get_tid=TID_UNDEF
    end select
  end function get_tid
  
  integer function get_next(p1,p2)
    integer,intent(out)::p1,p2
    integer k
    integer t
    
    k=cur_pos
    if(k>=len_expr.or.k<0) then
       get_next=TID_FIN
       return
    end if
    
    k=k+1
    p1=k
    p2=k
    t=get_tid(expr(k:k))
    if(t==TID_UNDEF) then
       if(is_alpha(ichar(expr(k:k)))) then
          t=TID_PARU
       else if(is_numeric(ichar(expr(k:k)))) then
          t=TID_FIG
       end if
    end if
    
    select case(t)
    case(TID_BOP1U)
       select case(old_tid)
       case(TID_BRA,TID_BOP3,TID_ASN) ! plus in (+, ^+ and e+ are unary
          t=TID_UOP2
       case default
          if(k==1) then
             t=TID_UOP2
          else
             t=TID_BOP1
          end if
       end select
    case(TID_BOP2U)
       if(k<len_expr) then
          k=k+1
          if(expr(k:k)=="*") then
             t=TID_BOP3
             p2=k
          end if
       end if
       if(t==TID_BOP2U) t=TID_BOP2
    case(TID_PARU)
       if(old_tid==TID_FIG.and.expr(k:k)=="e") then
          if(k<len_expr) then
             k=k+1
             select case(expr(k:k))
             case("+","-")
                if(k<len_expr) then
                   k=k+1
                   if(is_number(ichar(expr(k:k)))) t=TID_BOP3
                end if
             case default
                if(is_number(ichar(expr(k:k)))) t=TID_BOP3
             end select
          end if
       end if
       if(t==TID_PARU) then
          p2=get_end_of_par(p1)
          if(p2<len_expr) then
             k=p2+1
             if(expr(k:k)=="(") then
                t=TID_FNC ! maybe
             end if
          end if
          if(t==TID_PARU) t=TID_PAR
       end if
    case(TID_FIG)
       p2=get_end_of_fig(k)
       if(p2<0) t=TID_INV
    end select
    
    old_tid=t
    get_next=t
    cur_pos=p2
    
  end function get_next
  
  subroutine rpn_put(tid,p1,p2)
    integer,intent(in)::tid,p1,p2
    p_rpnq=p_rpnq+1
    rpnq(p_rpnq)%tid=tid
    rpnq(p_rpnq)%p1=p1
    rpnq(p_rpnq)%p2=p2
  end subroutine rpn_put

  subroutine rpn_push(tid,p1,p2)
    integer,intent(in)::tid,p1,p2
    p_buf=p_buf+1
    buf(p_buf)%tid=tid
    buf(p_buf)%p1=p1
    buf(p_buf)%p2=p2
  end subroutine rpn_push

  subroutine rpn_pop()
    if(p_buf<0) return
    p_rpnq=p_rpnq+1
    rpnq(p_rpnq)=buf(p_buf)
    p_buf=p_buf-1
  end subroutine rpn_pop

  subroutine rpn_pop_all()
    integer i
    do i=p_buf,1,-1
       if(buf(i)%tid==TID_BRA) cycle ! skip unclosed bra
       p_rpnq=p_rpnq+1
       rpnq(p_rpnq)=buf(i)
    end do
    p_buf=0
  end subroutine rpn_pop_all

  subroutine rpn_try_pop()
    do
       if(p_buf<=0) exit
       if(buf(p_buf)%tid/=TID_BRA) then
          call rpn_pop()
       else
          p_buf=p_buf-1
          exit
       end if
    end do
  end subroutine rpn_try_pop

  subroutine rpn_try_push(tid,p1,p2)
    integer,intent(in)::tid,p1,p2
    if(p_buf==0) then
       call rpn_push(tid,p1,p2)
    else
       do 
          if(p_buf<=0) exit
          if(buf(p_buf)%tid>=tid) then
             call rpn_pop()
          else
             exit
          end if
       end do
       call rpn_push(tid,p1,p2)
    end if
  end subroutine rpn_try_push

  subroutine dump_rpnq()
    integer i
    do i=1,p_rpnq
       write(*,10) i,rpnq(i)%tid,rpnq(i)%p1,rpnq(i)%p2
10     format(4(x,i4),x,$)
       if(rpnq(i)%tid==TID_VAR) then
          write(*,*) rbuf(rpnq(i)%p1)
       else if(rpnq(i)%p1==0) then
          write(*,*) "*"
       else if(rpnq(i)%tid/=TID_NOP) then
          write(*,*) _EXPR_(i)
       else
          write(*,*)
       end if
    end do
  end subroutine dump_rpnq

  integer function build_rpnq()
    integer t,told,istat
    integer p1,p2
    told=TID_NOP
    istat=0
    do 
       t=get_next(p1,p2)
       select case(t)
       case(TID_FIN)
          call rpn_pop_all()
          exit
       case(TID_INV)
          write(*,*) "*** Syntacs Error at: "
          write(*,*) trim(expr)
          write(*,*) repeat(" ",p1-1)//trim("1")
          istat=-1
          exit
       case(TID_PAR)
          select case(told)
          case(TID_FIG,TID_KET)
             call rpn_try_push(TID_BOP4,0,0)
        end select
        call rpn_put(t,p1,p2)
     case(TID_FIG)
        if(told==TID_KET) call rpn_try_push(TID_BOP4,0,0)
        call rpn_put(t,p1,p2)
     case(TID_UOP1,TID_UOP2,TID_BOP1,TID_BOP2,TID_BOP3)
        call rpn_try_push(t,p1,p2)
     case(TID_FNC)
        select case(told)
        case(TID_KET,TID_FIG)
           call rpn_try_push(TID_BOP4,0,0)
        end select
        call rpn_try_push(t,p1,p2)
     case(TID_BRA)
        select case(told)
        case(TID_FIG,TID_PAR)
           call rpn_try_push(TID_BOP4,0,0)
        end select
        call rpn_push(t,p1,p2)
     case(TID_KET)
        call rpn_try_pop()
     case(TID_ASN)
        call rpn_try_push(t,p1,p2)
        call rpn_push(TID_BRA,0,0) !<<<<<<<<<<<
     end select
     told=t
  end do
  build_rpnq=istat
end function build_rpnq

end module rpn

program ucalc
  use rpn
  use plist
  implicit none
  integer n,len
  character*16384 str
  integer istat
  logical drpnq

  call init_par()

  n=command_argument_count()
  if(n>0) then
     call get_command_argument(1,str,len)  
     call init_rpn(str)
     istat=build_rpnq()
     call dump_rpnq()
     istat=rpn_eval()
     write(*,*) "rpn_eval=",istat
     call dump_rpnq()
     write(*,*) "ans=",rpn_ans()
  else
     do
        drpnq=.false.
        write(*,10) "> "
10      format(x,a,$)
        read(*,"(a)") str
        if(str=="") cycle
        select case(str)
        case("quit","exit","q")
           exit
        case("pdump","pd")
           call print_pars()
           cycle
        case("qdump","qd")
           call dump_rpnq()
           cycle
        end select
        if(str(1:4)=="dump") then
           drpnq=.true.
           str(1:4)=""
        end if
        call init_rpn(str)
        istat=build_rpnq()
        if(istat/=0) then
           write(*,*)  "*** build_rpn failed: code = ",istat
           call dump_rpnq()
        else
           if(drpnq) then
              write(*,*) "before eval:"
              call dump_rpnq()             
           end if
           istat=rpn_eval()
           if(istat/=0) then
              write(*,*) "*** rpn_eval failed: code = ",istat
              call dump_rpnq()
           else
              if(drpnq) then
                 write(*,*) "after eval:"
                 call dump_rpnq()
              end if
              write(*,*) rpn_ans()
           end if
        end if
     end do
  end if
  
end program ucalc
