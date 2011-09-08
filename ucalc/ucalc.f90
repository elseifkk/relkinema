module plist
implicit none

type t_slist
   integer sz ! total size
   integer st ! stack top
   integer n  ! num elements
   integer p  ! str list
end type t_slist

type t_plist
   type(t_slist) s
   complex*16,allocatable::v(:)
end type t_plist

type t_flist
   type(t_slist) s
   type(t_slist) f
end type t_flist

integer,parameter::PLERR_NOMEM = 1
integer,parameter::PLERR_MEMOV = 2
integer,parameter::PLERR_NOENT = 3
integer,parameter::PLERR_RDONL = 4

integer,parameter::LEN_SLIST_HDR=2 ! len+code

integer*1,parameter::SC_NOP = 0
integer*1,parameter::SC_RO  = Z"01"
integer*1,parameter::SC_REF = Z"02"
!
integer*1,parameter::SC_MAC = Z"02"
integer*1,parameter::SC_FNC = Z"04"

interface add_par_by_value
   module procedure add_par_by_value_r
   module procedure add_par_by_value_c
end interface add_par_by_value

contains

  subroutine min_cp_slist(sl1,sl2)
    type(t_slist),intent(in)::sl1
    type(t_slist),intent(out)::sl2
    integer sz
    sz=sl1%st-sl1%p+1
    if(sl2%p/=0) call free(sl2%p) !<<<<
    if(sl1%n>0.and.sz>0) then
       sl2%p=malloc(sz)
       sl2%sz=sz
       call cpmem(sl2%p,sl1%p,sz)
       sl2%st=sl2%p+sz-1
       sl2%n=sl1%n
    else
       sl2%n=0
       sl2%p=0
       sl2%st=0
       sl2%sz=0
    end if
  end subroutine min_cp_slist

  subroutine min_cp_plist(pl1,pl2)
    type(t_plist),intent(in)::pl1
    type(t_plist),intent(out)::pl2
    if(allocated(pl2%v)) deallocate(pl2%v)
    if(allocated(pl1%v).and.size(pl1%v)>0) then
       allocate(pl2%v(size(pl1%v)))
       pl2%v=pl1%v
    end if
    call min_cp_slist(pl1%s,pl2%s)
  end subroutine min_cp_plist

  function init_slist(sz)
    type(t_slist) init_slist
    integer,intent(in)::sz
    init_slist%p=malloc(sz)
    init_slist%sz=sz
    init_slist%st=init_slist%p-1
    init_slist%n=0
  end function init_slist
 
 function init_plist(sz,nmax)
    type(t_plist) init_plist
    integer,intent(in)::sz
    integer,intent(in)::nmax
    init_plist%s=init_slist(sz)
    allocate(init_plist%v(nmax))
  end function init_plist
  
  function init_flist(sz,fsz)
    type(t_flist) init_flist
    integer,intent(in)::sz
    integer,intent(in)::fsz
    init_flist%s=init_slist(sz)
    init_flist%f=init_slist(fsz)
  end function init_flist

  subroutine dump_plist(pl)
    type(t_plist),intent(in)::pl
    integer i,len
    integer*1 code
    character*256 s
    integer*1 b
    integer si
    complex*16 v
    pointer(rp,v)
    pointer(p,b)
    pointer(ptr,si)
    p=pl%s%p-1
    do i=1,pl%s%n
       p=p+1
       len=b
       p=p+1
       code=b
       s=cpstr(p+1,len)
       write(*,10) i,code,s(1:len)
       if(is_reference(code)) then
          ptr=loc(pl%v(i))
          rp=si
          write(*,20) si
          write(*,*) v
       else
          write(*,*) pl%v(i)
       end if
       p=p+len
    end do
20  format(x,z16,$)
10  format(x,i4,x,b4.4,x,a,$)
  end subroutine dump_plist

  character*256 function cpstr(ptr,len)
    integer,intent(in)::ptr
    integer,intent(in)::len
    integer i
    character*1 c
    pointer(p,c)
    cpstr=""
    p=ptr-1
    do i=1,len
       p=p+1
       cpstr(i:i)=c
    end do
  end function cpstr

  subroutine trim_plist(pl)
    type(t_plist),intent(inout)::pl
    complex*16,allocatable::tmpv(:)
    integer tmpp
    integer sz

    if(allocated(pl%v)) then
       if(pl%s%n>=1) then
          allocate(tmpv(pl%s%n))
          tmpv(1:pl%s%n)=pl%v(1:pl%s%n)
          deallocate(pl%v)
          allocate(pl%v(pl%s%n))
          pl%v(1:pl%s%n)=tmpv(1:pl%s%n)
          deallocate(tmpv)
       else
          deallocate(pl%v)
       end if
    end if

    sz=pl%s%st-pl%s%p+1
    if(sz>=0) then
       tmpp=malloc(pl%s%st)
       call cpmem(tmpp,pl%s%p,sz)
       call free(pl%s%p)
       pl%s%p=malloc(sz)
       call cpmem(pl%s%p,tmpp,sz)
       call free(tmpp)
       pl%s%sz=pl%s%sz
    else if(pl%s%p/=0) then
       call free(pl%s%p)
       pl%s%p=0
       pl%s%st=0
       pl%s%n=0
       pl%s%sz=0
    end if
    
   end subroutine trim_plist
  
  subroutine cpmem(dst,src,len)
    integer,intent(in)::dst,src,len
    integer*1 s,d
    integer i
    pointer(si,s)
    pointer(di,d)
    si=src-1
    di=dst-1
    do i=1,len
       si=si+1
       di=di+1
       d=s
    end do
  end subroutine cpmem
    
  integer function rm_par(pl,s)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer istat
    integer k
    integer i
    istat=rm_str(pl%s,s,ent=k)
    if(istat/=0) then
       rm_par=istat
       return
    end if
    do i=k,pl%s%n
       pl%v(i)=pl%v(i+1)
    end do
    rm_par=0
  end function rm_par
  
  integer function find_par(pl,s,val,ent)
    type(t_plist),intent(in)::pl
    character*(*),intent(in)::s
    complex*16,intent(out),optional::val
    integer,intent(out),optional::ent
    integer k
    integer*1 c
    integer si
    complex*16 x
    pointer(ptr,si)
    pointer(pr,x)
    if(present(ent)) ent=0
    k=find_str(pl%s,s,found_code=c)
    if(k==0) then
       find_par=PLERR_NOENT
       return
    end if
    if(present(val)) then
       if(is_reference(c)) then
          ptr=loc(pl%v(k))
          pr=si
          val=x
       else
          val=pl%v(k)
       end if
    end if
    if(present(ent)) ent=k
    find_par=0
  end function find_par

  integer function get_str_ptr(sl,k,ptr,len)
    type(t_slist),intent(in)::sl
    integer,intent(in)::k
    integer,intent(out)::ptr
    integer,intent(out),optional::len
    integer*1 b
    integer l
    integer i
    pointer(p,b)
    if(present(len)) len=0
    if(sl%n<k.or.k<=0) then
       get_str_ptr=PLERR_NOENT
       return
    end if
    p=sl%p
    do i=1,k-1
       l=b
       p=p+l+LEN_SLIST_HDR
    end do
    ptr=p
    if(present(len)) len=b
    get_str_ptr=0
  end function get_str_ptr

  integer function get_sc(sl,k,code)
    type(t_slist),intent(in)::sl
    integer,intent(in)::k
    integer*1,intent(out)::code
    integer istat
    integer*1 b
    pointer(p,b)
    istat=get_str_ptr(sl,k,p)
    if(istat/=0) then
       get_sc=istat
       return
    end if
    p=p+1
    code=b
    get_sc=0
  end function get_sc
  
  logical function is_read_only(c)
    integer*1,intent(in)::c
    is_read_only=(iand(c,1)/=0)
  end function is_read_only

  logical function is_reference(c)
    integer*1,intent(in)::c
    is_reference=(iand(c,2)/=0)
  end function is_reference

  integer function put_par(pl,k,v)
    type(t_plist),intent(inout)::pl
    integer,intent(in)::k
    complex*16,intent(in)::v
    integer istat
    integer*1 c
    integer p,l
    istat=get_sc(pl%s,k,c)
    if(istat/=0) then
       put_par=istat
       return
    end if
    if(is_read_only(c)) then
       istat=get_str_ptr(pl%s,k,p,l)
       write(*,*) "*** Parameter is read-only: "//trim(cpstr(p+LEN_SLIST_HDR,l))
       put_par=PLERR_RDONL
       return
    end if
    if(.not.is_reference(c)) then
       pl%v(k)=v
    else
       call put_par_by_reference(pl,k,v)
    end if
    put_par=0
  end function put_par

  subroutine put_par_by_reference(pl,k,v)
    type(t_plist),intent(in)::pl
    integer,intent(in)::k
    complex*16,intent(in)::v
    integer ptr
    complex*16 x
    pointer(pptr,ptr)
    pointer(px,x)
    pptr=loc(pl%v(k))
    px=ptr
    x=v
  end subroutine put_par_by_reference

  integer function change_sc(sl,k,code)
    type(t_slist),intent(in)::sl
    integer,intent(in)::k
    integer*1,intent(in)::code
    integer*1 b
    integer istat
    pointer(p,b)
    istat=get_str_ptr(sl,k,p)
    if(istat/=0) then
       change_sc=istat
       return
    end if
    p=p+1
    b=code
    change_sc=0
  end function change_sc

  subroutine inc_par_buf(pl,inc_n)
    type(t_plist),intent(inout)::pl
    integer,intent(in)::inc_n
    complex*16,allocatable::v(:)
    if(inc_n<=0) return
    if(allocated(pl%v)) then
       allocate(v(size(pl%v)+inc_n))
       v(1:size(pl%v))=pl%v
       deallocate(pl%v)
       allocate(pl%v(size(v)))
       pl%v=v
       deallocate(v)
    else
       allocate(pl%v(inc_n))
    end if
  end subroutine inc_par_buf

  integer function try_add_par(pl,s,code,ent)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer*1,intent(in)::code
    integer,intent(out),optional::ent
    integer istat
    istat=try_add_str(pl%s,s,code,ent)
    if(istat/=0) then
       try_add_par=istat
       return
    end if
    if(pl%s%n>size(pl%v).or..not.allocated(pl%v)) call inc_par_buf(pl,4)
    try_add_par=0
  end function try_add_par

  integer function add_par_by_entry(pl,s,ent,ro)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer,intent(out)::ent
    logical,intent(in),optional::ro
    integer*1 c
    if(present(ro).and.ro) then
       c=SC_RO
    else
       c=0
    end if
    add_par_by_entry=try_add_par(pl,s,c,ent)
  end function add_par_by_entry

  integer function add_par_by_reference(pl,s,ptr,ro,ent)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    integer,intent(in)::ptr
    logical,intent(in),optional::ro
    integer,intent(out),optional::ent
    integer istat
    integer k
    integer si !<<<<<<<<< size undetermined
    integer*1 c
    pointer(p,si)
    if(present(ent)) ent=0
    if(present(ro).and.ro) then
       c=ior(SC_REF,SC_RO)
    else
       c=SC_REF
    end if
    istat=try_add_par(pl,s,c,k)
    if(istat/=0) then
       add_par_by_reference=istat
       return
    end if
    pl%v(k)=0
    p=loc(pl%v(k))
    si=ptr
    if(present(ent)) ent=k
    add_par_by_reference=0
  end function add_par_by_reference

  integer function add_par_by_value_r(pl,s,v,ro,ent)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    real*8,intent(in)::v
    logical,intent(in),optional::ro
    integer,intent(out),optional::ent
    complex*16 z
    z=complex(v,0.)
    add_par_by_value_r=add_par_by_value_c(pl,s,z,ro,ent)
  end function add_par_by_value_r

  integer function add_par_by_value_c(pl,s,v,ro,ent)
    type(t_plist),intent(inout)::pl
    character*(*),intent(in)::s
    complex*16,intent(in)::v
    logical,intent(in),optional::ro
    integer,intent(out),optional::ent
    integer istat
    integer k
    integer*1 c
    if(present(ent)) ent=0
    if(present(ro).and.ro) then
       c=SC_RO
    else
       c=SC_NOP
    end if
    istat=try_add_par(pl,s,c,k)
    if(istat/=0) then
       add_par_by_value_c=istat
       return
    end if
    pl%v(k)=v
    if(present(ent)) ent=k
    add_par_by_value_c=0
  end function add_par_by_value_c

  integer function rm_str(sl,s,ent,flg)
    type(t_slist),intent(inout)::sl
    character*(*),intent(in)::s
    integer,intent(out),optional::ent
    integer,intent(in),optional::flg
    integer k
    integer*1 cd
    integer ptr
    k=find_str(sl,s,found_code=cd,ptr=ptr)
    if(present(ent)) ent=k
    if(k==0) then
       rm_str=PLERR_NOENT
       return
    end if
    if(is_read_only(cd)) then
       rm_str=PLERR_RDONL
       return
    end if
    call shift_slist()
    sl%n=sl%n-1
    rm_str=0
  contains
    subroutine shift_slist()
      integer*1 c,d
      pointer(si,c)
      pointer(di,d)
      di=ptr
      si=ptr+d+LEN_SLIST_HDR
      if(si>sl%st) then
         sl%st=di-1
      else
         do 
            d=c
            if(si==sl%st) exit
            di=di+1
            si=si+1
         end do
         sl%st=di
      end if
    end subroutine shift_slist
  end function rm_str

  integer function try_add_str(sl,s,code,ent)
    type(t_slist),intent(inout)::sl
    character*(*),intent(in)::s
    integer*1,intent(in)::code
    integer,intent(out),optional::ent
    integer k,istat
    integer*1 c
    k=0
    istat=0
    if(sl%n>0) then
       k=find_str(sl,s,found_code=c)
       if(k/=0) then
          if(present(ent)) ent=k
          if(is_read_only(c)) then
             istat=PLERR_RDONL
             write(*,*) "*** Parameter is read-only: "//trim(s)
          else if(c/=code) then
             istat=change_sc(sl,k,code)
          end if
       end if
    end if
    if(k==0.and.istat==0) istat=add_str(sl,s,code,ent)
    try_add_str=istat
  end function try_add_str

  subroutine inc_slist(sl,inc_sz)
    type(t_slist),intent(inout)::sl
    integer,intent(in)::inc_sz
    integer p
    integer std
    if(inc_sz<=0) return
    p=malloc(sl%sz+inc_sz)
    std=sl%st-sl%p
    if(sl%p/=0) then
       call cpmem(p,sl%p,sl%sz)
       call free(sl%p)
    end if
    sl%p=p
    sl%st=std+p
    sl%sz=sl%sz+inc_sz
  end subroutine inc_slist

  integer function add_str(sl,s,code,ent)
    type(t_slist),intent(inout)::sl
    character*(*),intent(in)::s
    integer*1,intent(in)::code
    integer,intent(out),optional::ent
    integer i
    integer*1 b
    integer*1 c
    pointer(q,c)
    pointer(p,b)
    integer len
    if(present(ent)) ent=0
    if(sl%p==0) then !<<<<<<<<<<<<<<<<<
       add_str=PLERR_NOMEM
       return
    end if
    len=len_trim(s)
    if(len>255) then
       add_str=PLERR_MEMOV
       return
    end if
    if(len+LEN_SLIST_HDR+(sl%st-sl%p+1)>sl%sz) &
         call inc_slist(sl,len+LEN_SLIST_HDR)
    p=sl%st+1
    b=int(len,kind=1)
    p=p+1
    b=code
    q=loc(s)-1
    do i=1,len
       p=p+1
       q=q+1
       b=c
    end do
    sl%st=p
    sl%n=sl%n+1 
    add_str=0
    if(present(ent)) ent=sl%n
  end function add_str

  integer function find_str(sl,s,target_code,found_code,ptr)
    type(t_slist),intent(in)::sl
    character*(*),intent(in)::s
    integer*1,intent(in),optional::target_code
    integer*1,intent(out),optional::found_code
    integer,intent(out),optional::ptr
    integer i
    integer len
    integer lenp
    integer*1 cd
    integer*1 b
    integer*1 tc
    pointer(p,b)
    find_str=0
    p=sl%p
    len=len_trim(s)
    if(present(target_code)) then
       tc=target_code
    else
       tc=-1
    end if
    do i=1,sl%n
       lenp=b
       if(len==lenp) then
          if(is_matched()) then
             find_str=i
             if(present(found_code)) found_code=cd
             if(present(ptr)) ptr=p
             return
          end if
       end if
       p=p+lenp+LEN_SLIST_HDR
    end do
  contains
    logical function is_matched()
      integer*1 c
      integer j
      pointer(q,c)
      is_matched=.false.
      q=p+1 ! skip code
      cd=c
      if(tc/=-1.and.tc/=c) return
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

!  private

  integer,parameter::RPNERR_NOENT  =  1
  integer,parameter::RPNERR_NOOP   =  2
  integer,parameter::RPNERR_NOPAR  =  3
  integer,parameter::RPNERR_NOFNC  =  4
  integer,parameter::RPNERR_ADDPAR =  5
  integer,parameter::RPNERR_INVASN =  6
  integer,parameter::RPNERR_INVOP  =  7
  integer,parameter::RPNERR_INVFNC =  8
  integer,parameter::RPNERR_PARSER =  9
  integer,parameter::RPNERR_ADDSTR = 10
  integer,parameter::RPNERR_MEMOV  = 11
  integer,parameter::RPNERR_RECOV  = 12

  integer,parameter::RPN_REC_MAX   =  256
  integer,parameter::NUM_VBUF_MIN  =   32
  integer,parameter::NUM_PBUF_MIN  =   32
  integer,parameter::NUM_RPNM_MIN  =    8
  integer,parameter::LEN_PLIST_MIN = 1024
  integer,parameter::LEN_RLIST_MIN = 1024

  ! meta tid
  integer,parameter::TID_FIN   = -999
  integer,parameter::TID_UNDEF =  0
  integer,parameter::TID_INV   =  -666
  ! operators
  integer,parameter::TID_ASN   =  1  ! =
  integer,parameter::TID_AOP   =  2 
  integer,parameter::TID_BOP1  =  3  ! +,-
  integer,parameter::TID_BOP1U = -3
  integer,parameter::TID_BOP2  =  4  ! *,/
  integer,parameter::TID_BOP2U = -4  ! *,/
  integer,parameter::TID_BOP4  =  5  ! implicit * <<<<<<<<<<< 
  integer,parameter::TID_BOP3  =  6  ! ^,**,e
  integer,parameter::TID_BOP3U = -6  ! ^,**,e
  integer,parameter::TID_UOP1  =  7  ! +,-
  integer,parameter::TID_UOP2  =  8  ! !
  integer,parameter::TID_IFNC  =  9  ! sin, cos,...
  integer,parameter::TID_UFNC  = 10  ! sin, cos,...
  ! braket
  integer,parameter::TID_IBRA  = -11   ! implicit (
  integer,parameter::TID_BRA   = -12   ! (
  integer,parameter::TID_KET   = -13  ! )
  integer,parameter::TID_QTN   = -14  ! "
  integer,parameter::TID_QEND  = -15
  integer,parameter::TID_QSTA  = -16
  integer,parameter::TID_COMA  = -17  ! ,
  integer,parameter::TID_MASN  = -18  ! = for macro
  ! variables
  integer,parameter::TID_PAR   =  19  ! a,b,c,...
  integer,parameter::TID_PARU  = -19  ! a,b,c,...
  integer,parameter::TID_FIG   =  20  ! 1,2,3,...
  integer,parameter::TID_VAR   =  21  ! fig in rbuf
  integer,parameter::TID_MAC   =  22
  integer,parameter::TID_OP1   =  23  ! operators
  integer,parameter::TID_OP2   =  24
  integer,parameter::TID_APAR  =  25  ! par assign
  integer,parameter::TID_AMAC  =  26
  integer,parameter::TID_AFNC  =  27
  integer,parameter::TID_DPAR  =  28  ! dummy par
  !

  ! do nothing or undef
  integer,parameter::TID_NOP   =  999

  type t_rrpnq
     integer tid
     integer p1
     integer p2
  end type t_rrpnq

  type t_rpnq
     integer tid
     integer cid ! oid or fid or pointer to value
  end type t_rpnq
  
  type t_rpnbuf
     character*16384 expr
     integer len_expr
     integer cur_pos
     integer old_pos
     integer old_tid
     type(t_rrpnq),allocatable::que(:)
     type(t_rrpnq),allocatable::buf(:)
     integer p_buf,p_que
  end type t_rpnbuf

  type t_rpnm
     type(t_rpnq),pointer::que(:)  ! allocated
     complex*16,pointer::vbuf(:)   ! allocated
     integer,pointer::p_vbuf       ! allocated
     type(t_plist),pointer::pars   ! => rpnc%pars
     complex*16,pointer::answer    ! => rpnc%answer
     complex*16,pointer::tmpans    ! => rpnc%tmpans
     type(t_slist),pointer::pnames
     integer,pointer::na ! num arg
  end type t_rpnm

  type t_rpnlist
     type(t_rpnm),allocatable::rpnm(:)
     type(t_slist) s
  end type t_rpnlist

  type t_rpnc
     type(t_rpnq),pointer::que(:)
     complex*16,pointer::vbuf(:)
     integer,pointer::p_vbuf
     type(t_plist),pointer::pars
     complex*16,pointer::answer
     complex*16,pointer::tmpans
     type(t_rpnlist),pointer::rl
     integer,pointer::rc ! recursion count
  end type t_rpnc

  integer,parameter::AID_NOP = 0
  integer,parameter::AID_MOV = 1
  integer,parameter::AID_INC = 2
  integer,parameter::AID_DEC = 3
  integer,parameter::AID_MUL = 4
  integer,parameter::AID_DIV = 5
  integer,parameter::AID_POW = 6
  
  integer,parameter::OID_NOP = 0
  integer,parameter::OID_NEG = 1
  integer,parameter::OID_ADD = 2 
  integer,parameter::OID_SUB = 3
  integer,parameter::OID_MUL = 4
  integer,parameter::OID_DIV = 5
  integer,parameter::OID_POW = 6
  integer,parameter::OID_EXP = 7
  integer,parameter::OID_FAC = 8

  integer,parameter::int_fnc_max_len=5
  character*(*),parameter::int_fnc=&
       achar(3)//"sin"//&
       achar(3)//"cos"//&
       achar(3)//"tan"//&
       achar(4)//"sinh"//&
       achar(4)//"cosh"//&
       achar(4)//"tanh"//&
       achar(4)//"asin"//&
       achar(4)//"acos"//&
       achar(4)//"atan"//&
       achar(5)//"asinh"//&
       achar(5)//"acosh"//&
       achar(5)//"atanh"//&
       achar(3)//"exp"//&
       achar(3)//"log"//&
       achar(2)//"ln"//&
       achar(4)//"sqrt"//&
       achar(3)//"abs"//&
       achar(3)//"int"//&
       achar(4)//"frac"//&
       achar(5)//"conjg"//&
       achar(4)//"nint"//&
       achar(0)
  integer,parameter::FID_SIN   =  1
  integer,parameter::FID_COS   =  2 
  integer,parameter::FID_TAN   =  3
  integer,parameter::FID_SINH  =  4
  integer,parameter::FID_COSH  =  5
  integer,parameter::FID_TANH  =  6
  integer,parameter::FID_ASIN  =  7
  integer,parameter::FID_ACOS  =  8
  integer,parameter::FID_ATAN  =  9
  integer,parameter::FID_ASINH = 10
  integer,parameter::FID_ACOSH = 11
  integer,parameter::FID_ATANH = 12
  integer,parameter::FID_EXP   = 13
  integer,parameter::FID_LOG   = 14
  integer,parameter::FID_LN    = 15
  integer,parameter::FID_SQRT  = 16
  integer,parameter::FID_ABS   = 17
  integer,parameter::FID_INT   = 18
  integer,parameter::FID_FRAC  = 19
  integer,parameter::FID_CONJG = 20
  integer,parameter::FID_NINT  = 21

contains

  complex*16 function rpn_ans(rpnc)
    type(t_rpnc),intent(in)::rpnc
    rpn_ans=rpnc%answer
  end function rpn_ans
  
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

  subroutine init_rpnlist(rl)
    type(t_rpnlist),intent(inout)::rl
    rl%s=init_slist(LEN_RLIST_MIN)
    allocate(rl%rpnm(NUM_RPNM_MIN))
  end subroutine init_rpnlist

  subroutine inc_vbuf(rpnc,n)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::n
    complex*16,allocatable::vb(:)
    if(n<=0) return
    if(associated(rpnc%vbuf).and.size(rpnc%vbuf)>0) then
       allocate(vb(size(rpnc%vbuf)))
       vb=rpnc%vbuf
       deallocate(rpnc%vbuf)
       allocate(rpnc%vbuf(size(vb)+n))
       rpnc%vbuf(1:size(vb))=vb
       deallocate(vb)
    else
       allocate(rpnc%vbuf(n))
    end if
  end subroutine inc_vbuf
  
  subroutine init_rpnc(rpnc)
    type(t_rpnc),intent(inout),target::rpnc
    nullify(rpnc%que)
    nullify(rpnc%vbuf)
    allocate(rpnc%vbuf(NUM_VBUF_MIN)) !<<<<<<<<<<<<<<<<<<<<<
    allocate(rpnc%rl)
    allocate(rpnc%tmpans)
    allocate(rpnc%answer)
    allocate(rpnc%pars)
    allocate(rpnc%p_vbuf)
    allocate(rpnc%rc)
    call init_par(rpnc)
    call init_rpnlist(rpnc%rl)
  end subroutine init_rpnc

  subroutine init_par(rpnc)
    type(t_rpnc),intent(inout)::rpnc
    integer istat
    rpnc%pars=init_plist(LEN_PLIST_MIN,NUM_PBUF_MIN)
    istat=add_par_by_reference(rpnc%pars,"tmp",loc(rpnc%tmpans),.true.)
    istat=add_par_by_reference(rpnc%pars,"ans",loc(rpnc%answer),.true.)
    istat=add_par_by_value(rpnc%pars,"i",complex(0.,1.),.true.)
    istat=add_par_by_value(rpnc%pars,"pi",atan(1.)*4.,.true.)
    istat=add_par_by_value(rpnc%pars,"c",2.99792458e8,.true.)
  end subroutine init_par

  subroutine delete_par(rpnc,s)
    type(t_rpnc),intent(inout)::rpnc
    character*(*),intent(in)::s
    integer istat
    istat=rm_par(rpnc%pars,trim(adjustl(s)))
    if(istat/=0) write(*,*) "*** Error delete_par: "//trim(s)//": code = ",istat
  end subroutine delete_par

  integer function get_operand(rpnc,i)
    type(t_rpnc),intent(in)::rpnc
    integer,intent(in)::i
    integer j
    get_operand=0
    do j=i,1,-1
       if(rpnc%que(j)%tid==TID_VAR) then
          get_operand=j
          return
       end if
    end do
  end function get_operand

  complex*16 function fac(v)
    complex*16,intent(in)::v
    integer i,n
    n=int(v)
    fac=1
    if(n<=1) return
    do i=2,n
       fac=fac*i
    end do
  end function fac

  integer function eval_1(rpnc,i)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    integer od
    complex*16 v1,v2
    pointer(pv1,v1)

    od=get_operand(rpnc,i-1)
    if(od==0) then
       eval_1=RPNERR_NOOP
       return
    end if
    pv1=rpnc%que(od)%cid
    select case(rpnc%que(i)%cid)
    case(OID_NOP)
    case(OID_NEG)
       v2=-v1
       if(abs(imagpart(v2))==0) then ! gfortran46 gives -0 for negation of 0
          v2=complex(realpart(v2),0.)
       end if
       if(abs(realpart(v2))==0) then 
          v2=complex(0.,imagpart(v2))
       end if
    case(OID_FAC)
       v2=fac(v1)
    end select
    rpnc%tmpans=v2
    call put_vbuf(rpnc,i,v2)
    rpnc%que(od)%tid=TID_NOP
    eval_1=0
    rpnc%tmpans=v2
  end function eval_1

  integer function eval_2a(rpnc,i)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    integer od1,od2    
    complex*16 v1,v2
    pointer(pv1,v1)
    pointer(pv2,v2)
    od2=get_operand(rpnc,i-1)
    if(od2<=1) then
       eval_2a=RPNERR_NOOP
       return
    end if
    od1=get_operand(rpnc,od2-1)
    if(od1==0) then
       eval_2a=RPNERR_NOOP
       return
    end if
    pv2=rpnc%que(od2)%cid
    pv1=rpnc%que(od1)%cid 

    ! pv1 => pars
    select case(rpnc%que(i)%cid)
    case(AID_MOV)
       v1=v2
    case(AID_INC)
       v1=v1+v2
    case(AID_DEC)
       v1=v1-v2
    case(AID_MUL)
       v1=v1*v2
    case(AID_DIV)
       v1=v1/v2
    case(AID_POW)
       v1=v1**v2
    end select
    rpnc%tmpans=v1
    call put_vbuf(rpnc,i,v1)
    rpnc%que(od2)%tid=TID_NOP
    rpnc%que(od1)%tid=TID_NOP
    eval_2a=0
  end function eval_2a

  integer function eval_2(rpnc,i)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    integer od1,od2
    complex*16 v1,v2,v3
    pointer(pv1,v1)
    pointer(pv2,v2)

    od2=get_operand(rpnc,i-1)
    if(od2<=1) then
       eval_2=RPNERR_NOOP
       return
    end if
    od1=get_operand(rpnc,od2-1)
    if(od1==0) then
       eval_2=RPNERR_NOOP
       return
    end if

    pv1=rpnc%que(od1)%cid
    pv2=rpnc%que(od2)%cid

    select case(rpnc%que(i)%cid)
    case(OID_ADD)
       v3=v1+v2
    case(OID_SUB)
       v3=v1-v2
    case(OID_MUL)
       v3=v1*v2
    case(OID_DIV)
       v3=v1/v2
    case(OID_POW)
       v3=v1**v2
    case(OID_EXP)
       v3=v1*10**v2
    end select
    rpnc%tmpans=v3
    call put_vbuf(rpnc,i,v3)
    rpnc%que(od1)%tid=TID_NOP
    rpnc%que(od2)%tid=TID_NOP
    eval_2=0
  end function eval_2

  recursive function eval_nf(rpnc,i) result(istat)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    integer istat
    type(t_rpnm),pointer::rpnm
    type(t_rpnc) fnc
    integer j
    integer kp,ko
    integer,allocatable::ods(:)
    complex*16 v
    pointer(pv,v)

    rpnm=>rpnc%rl%rpnm(rpnc%que(i)%cid)

    if(i-1<rpnm%na) then
       istat=RPNERR_NOOP
       return
    end if

    ko=0
    allocate(ods(rpnm%na))
    do j=1,i-1
       if(rpnc%que(j)%tid==TID_VAR) then
          ko=ko+1
          ods(ko)=j
          if(ko==rpnm%na) exit
       end if
    end do
    if(ko/=rpnm%na) then
       deallocate(ods)
       istat=RPNERR_NOOP
       return
    end if

    allocate(fnc%que(size(rpnm%que)))
    allocate(fnc%vbuf(NUM_VBUF_MIN))
    allocate(fnc%p_vbuf)
    fnc%que=rpnm%que
    fnc%p_vbuf=0

    fnc%pars=>rpnc%pars
    fnc%answer=>rpnc%answer
    fnc%tmpans=>rpnc%tmpans
    fnc%rl=>rpnc%rl
    fnc%rc=>rpnc%rc
    
    istat=0
    do j=1,size(fnc%que)
       select case(fnc%que(j)%tid)
       case(TID_FIG)
          fnc%que(j)%cid=loc(rpnm%vbuf(fnc%que(j)%cid))
          fnc%que(j)%tid=TID_VAR
       case(TID_PAR,TID_APAR)
          call set_par_ptr(kp)
          if(istat/=0) exit 
          fnc%que(j)%cid=loc(fnc%pars%v(kp))
          fnc%que(j)%tid=TID_VAR
       case(TID_DPAR)
          pv=rpnc%que(ods(fnc%que(j)%cid))%cid
          call put_vbuf(fnc,j,v)
       end select
    end do

    if(istat==0) istat=rpn_eval(fnc)
    
    if(istat==0) then
       rpnc%que(i)%tid=TID_VAR
       call put_vbuf(rpnc,i,fnc%answer)
       rpnc%tmpans=fnc%answer
      do j=1,rpnm%na
          rpnc%que(ods(j))%tid=TID_NOP
       end do
    end if

    deallocate(ods)
    deallocate(fnc%que)
    deallocate(fnc%vbuf)
    deallocate(fnc%p_vbuf)

  contains
    
    subroutine set_par_ptr(ent) 
      integer,intent(out)::ent
      integer ptr,len      
      istat=get_str_ptr(rpnm%pnames,fnc%que(j)%cid,ptr,len)
      if(istat/=0) stop "*** UNEXPECTED ERROR in set_par_ptr"
      istat=find_par(fnc%pars,trim(cpstr(ptr+LEN_SLIST_HDR,len)),ent=ent)
      if(istat/=0) then
         write(*,*) "*** No such parameter: "//trim(cpstr(ptr+LEN_SLIST_HDR,len))
      end if
    end subroutine set_par_ptr

  end function eval_nf
  
  integer function eval_1f(rpnc,i)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    integer od
    complex*16 v1,v2
    pointer(pv1,v1)

    od=get_operand(rpnc,i-1)
    if(od==0) then
       eval_1f=RPNERR_NOOP
       return
    end if
    pv1=rpnc%que(od)%cid
    select case(rpnc%que(i)%cid)
    case(FID_SIN)
       v2=sin(v1)
    case(FID_COS)
       v2=cos(v1)
    case(FID_TAN)
       v2=tan(v1)
    case(FID_ASIN)
       v2=asin(v1)
    case(FID_ACOS)
       v2=acos(v1)
    case(FID_ATAN)
       v2=atan(v1)
    case(FID_EXP)
       v2=exp(v1)
    case(FID_SQRT)
       v2=sqrt(v1)
    case(FID_LN)
       v2=log(v1)
    case(FID_LOG)
       v2=log(v1)/log(10.)
    case(FID_SINH)
       v2=sinh(v1)
    case(FID_COSH)
       v2=cosh(v1)
    case(FID_TANH)
       v2=tanh(v1)
    case(FID_ASINH)
       v2=log(v1+sqrt(v1*v1+1))
    case(FID_ACOSH)
       v2=log(v1+sqrt(v1*v1-1))
    case(FID_ATANH)
       v2=log((1+v1)/(1-v1))/2.
    case(FID_ABS)
       v2=abs(v1)
    case(FID_INT)
       v2=int(v1)
    case(FID_FRAC)
       v2=v1-int(v1)
    case(FID_CONJG)
       v2=conjg(v1)
       if(abs(imagpart(v2))==0)  then
          v2=complex(realpart(v2),0.)
       end if
    case(FID_NINT)
       v2=int(v1+0.5)
    end select
    rpnc%tmpans=v2
    call put_vbuf(rpnc,i,v2)
    rpnc%que(od)%tid=TID_NOP
    eval_1f=0
  end function eval_1f

  subroutine put_vbuf(rpnc,i,v)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    complex*16,intent(in)::v
    if(rpnc%p_vbuf>=size(rpnc%vbuf)) call inc_vbuf(rpnc,NUM_VBUF_MIN)
    rpnc%p_vbuf=rpnc%p_vbuf+1
    rpnc%vbuf(rpnc%p_vbuf)=v
    rpnc%que(i)%cid=loc(rpnc%vbuf(rpnc%p_vbuf))
    rpnc%que(i)%tid=TID_VAR
  end subroutine put_vbuf

  recursive function eval_m(rpnc,i) result(istat)
    type(t_rpnc),intent(inout)::rpnc
    integer,intent(in)::i
    integer istat
    type(t_rpnm),pointer::rpnm
    type(t_rpnc) mac
    integer j
    integer kp

    rpnm=>rpnc%rl%rpnm(rpnc%que(i)%cid)
    
    allocate(mac%que(size(rpnm%que)))
    allocate(mac%vbuf(NUM_VBUF_MIN))
    allocate(mac%p_vbuf)
    mac%que    = rpnm%que
    mac%p_vbuf = 0

    mac%pars   => rpnc%pars
    mac%answer => rpnc%answer
    mac%tmpans => rpnc%tmpans
    mac%rl     => rpnc%rl
    mac%rc     => rpnc%rc

    istat=0
    do j=1,size(mac%que)
       select case(mac%que(j)%tid) 
       case(TID_PAR,TID_APAR)
          call set_par_ptr(kp)
          if(istat/=0) exit
          mac%que(j)%cid=loc(mac%pars%v(kp))
          mac%que(j)%tid=TID_VAR
       case(TID_FIG)
          mac%que(j)%cid=loc(rpnm%vbuf(mac%que(j)%cid))
          mac%que(j)%tid=TID_VAR
       end select
    end do

    if(istat==0) istat=rpn_eval(mac)

    if(istat==0) then
       rpnc%tmpans=mac%answer
       call put_vbuf(rpnc,i,mac%answer)
    end if

    deallocate(mac%que)
    deallocate(mac%vbuf)
    deallocate(mac%p_vbuf)
 
  contains

    subroutine set_par_ptr(ent)
      integer,intent(out)::ent
      integer ptr,len
      istat=get_str_ptr(rpnm%pnames,mac%que(j)%cid,ptr,len)
      if(istat/=0) stop "*** UNEXPECTED ERROR in load_par: get_str_ptr"
      istat=find_par(mac%pars,trim(cpstr(ptr+LEN_SLIST_HDR,len)),ent=ent)
      if(istat/=0) then
         write(*,*) "*** No such parameter: "//trim(cpstr(ptr+LEN_SLIST_HDR,len))
         istat=RPNERR_NOPAR
      end if
    end subroutine set_par_ptr

  end function eval_m

  recursive function rpn_eval(rpnc) result(istat)
    type(t_rpnc),intent(inout),target::rpnc
    integer i,istat,ec
    complex*16 v
    pointer(pv,v)
    istat=0
    ec=0
    if(rpnc%rc>RPN_REC_MAX) then
       istat=RPNERR_RECOV
       return
    end if
    rpnc%rc=rpnc%rc+1
    do i=1,size(rpnc%que)
       ec=ec+1
       select case(rpnc%que(i)%tid)
       case(TID_OP1)
          istat=eval_1(rpnc,i)
       case(TID_OP2)
          istat=eval_2(rpnc,i)
       case(TID_IFNC)
          istat=eval_1f(rpnc,i)
       case(TID_ASN)
          istat=eval_2a(rpnc,i)
       case(TID_MAC)
          istat=eval_m(rpnc,i)
       case(TID_UFNC)
          istat=eval_nf(rpnc,i)
       case default
          ec=ec-1
       end select
       if(istat/=0) then
          write(*,*) "*** Error in rpn_eval at que = ", i
          exit
       end if
    end do
    
    if(istat/=0) return

    if(ec==0.and.size(rpnc%que)==1) then ! only fig or par
       pv=rpnc%que(1)%cid
       rpnc%answer=v
    else
       rpnc%answer=rpnc%tmpans
    end if
    
    rpnc%rc=rpnc%rc-1

  end function rpn_eval

  integer function get_lo32(cid)
    integer,intent(in)::cid
    get_lo32=iand(cid,int(Z"FFFF",kind=4))
  end function get_lo32

  integer function get_up32(cid)
    integer,intent(in)::cid
    get_up32=iand(ishft(cid,-16),int(Z"FFFF",kind=4))
  end function get_up32

  integer function get_i32(lo,up)
    integer,intent(in)::lo,up
    get_i32=ior(lo,ishft(up,16))
  end function get_i32

  integer function get_end_of_fig(rpnbuf,k_)
    type(t_rpnbuf),intent(in)::rpnbuf
    integer,intent(in)::k_
    integer c
    integer a
    integer k
    
    k=k_
    if(rpnbuf%expr(k:k)==".") then
       c=1
    else
       c=0
    end if
    
    do
       if(k>=rpnbuf%len_expr) then
          get_end_of_fig=k
          return
       end if
       k=k+1
       a=ichar(rpnbuf%expr(k:k))
       if(.not.is_numeric(a)) then
          if(c==1.and.k-1==k_) then
             get_end_of_fig=-k_ ! only .
          else
             get_end_of_fig=k-1
          end if
          return
       else if(rpnbuf%expr(k:k)==".") then
          c=c+1
          if(c>1) then
             get_end_of_fig=-k ! second . found 
             return
          end if
       end if
    end do
    
  end function get_end_of_fig
  
  integer function get_end_of_par(rpnbuf,k_)
    type(t_rpnbuf),intent(in)::rpnbuf
    integer,intent(in)::k_
    integer k
    integer a
    k=k_
    do
       if(k>=rpnbuf%len_expr) then
          get_end_of_par=k
          return
       end if
       k=k+1
       a=ichar(rpnbuf%expr(k:k))
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
  
  integer function is_int_fnc(a)
    character*(*),intent(in)::a
    integer len
    integer k,j
    integer lenf
    integer i
    len=len_trim(a)
    is_int_fnc=0
    k=1
    i=0
    do
       i=i+1
       lenf=ichar(int_fnc(k:k))
       if(lenf==0) exit
       if(lenf==len) then
          j=k+1
          if(int_fnc(j:j+len-1)==a(1:len)) then
             is_int_fnc=i
             return
          end if
       end if
       k=k+lenf+1
    end do
  end function is_int_fnc

  integer function get_tid(a)
    character*1,intent(in)::a
    get_tid=0 ! in order to avoid warning
    select case(a)
    case("+","-")
       get_tid=TID_BOP1U
    case("*","/")
       get_tid=TID_BOP2U
    case("!")
       get_tid=TID_UOP2
    case("^")
       get_tid=TID_BOP3U
    case("(")
       get_tid=TID_BRA
    case(")")
       get_tid=TID_KET
    case("=")
       get_tid=TID_ASN
    case("""")
       get_tid=TID_QTN
    case(",")
       get_tid=TID_COMA
    case default
       get_tid=TID_UNDEF
    end select
  end function get_tid

  logical function is_user_fnc(sl,f)
    type(t_slist),intent(in)::sl
    character*(*),intent(in)::f
    if(find_str(sl,f,target_code=SC_FNC)/=0) then
       is_user_fnc=.true.
    else
       is_user_fnc=.false.
    end if
  end function is_user_fnc
  
  integer function get_next(rpnbuf,p1,p2,sl)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer,intent(out)::p1,p2
    type(t_slist),intent(in)::sl
    integer k
    integer t
    
    k=rpnbuf%cur_pos
    if(k>=rpnbuf%len_expr.or.k<0) then
       get_next=TID_FIN
       return
    end if
    
    k=k+1
    p1=k
    p2=k
    t=get_tid(rpnbuf%expr(k:k))
    if(t==TID_UNDEF) then
       if(is_alpha(ichar(rpnbuf%expr(k:k)))) then
          t=TID_PARU
       else if(is_numeric(ichar(rpnbuf%expr(k:k)))) then
          t=TID_FIG
       end if
    end if
    
    select case(t)
    case(TID_BOP1U)
       if(k<rpnbuf%len_expr-1) then
          if(next_char(1)=="=") then             
             t=TID_AOP
             p2=k+1
          end if
       end if
       if(t==TID_BOP1U) then
          select case(rpnbuf%old_tid)
          case(TID_BRA,TID_BOP3,TID_ASN,TID_AOP,TID_COMA) ! plus in (+, ^+ and e+ are unary
             t=TID_UOP1
          case(TID_UOP1,TID_BOP2,TID_BOP1)
             t=TID_INV
          case default
             if(k==1) then
                t=TID_UOP1
             else
                t=TID_BOP1
             end if
          end select
       end if
    case(TID_BOP2U)
       if(k<rpnbuf%len_expr-1) then
          select case(next_char(1))
          case("=")
             t=TID_AOP
             p2=k+1
          case("*")
             if(rpnbuf%expr(k:k)=="*") then
                t=TID_BOP3
                p2=k+1
             end if
          end select
       end if
       if(t==TID_BOP2U) t=TID_BOP2
    case(TID_BOP3U)
       if(k<rpnbuf%len_expr-1.and.next_char(1)=="=") then
          t=TID_AOP
          p2=k+1
       else
          t=TID_BOP3
       end if
    case(TID_PARU)
       if(rpnbuf%old_tid==TID_FIG.and.rpnbuf%expr(k:k)=="e") then
          if(k<rpnbuf%len_expr) then
             select case(next_char(1))
             case("+","-")
                if(k+1<rpnbuf%len_expr) then
                   if(is_number(ichar(next_char(2)))) t=TID_BOP3
                end if
             case default
                if(is_number(ichar(next_char(1)))) t=TID_BOP3
             end select
          end if
       end if
       if(t==TID_PARU) then
          p2=get_end_of_par(rpnbuf,p1)
          if(p2<rpnbuf%len_expr) then
             k=p2+1
             if(rpnbuf%expr(k:k)=="(") then
                if(is_int_fnc(rpnbuf%expr(p1:p2))/=0) then
                   t=TID_IFNC ! intrinsic func
                else if(is_user_fnc(sl,rpnbuf%expr(p1:p2))) then
                   t=TID_UFNC
                end if
             end if
          end if
          if(t==TID_PARU) t=TID_PAR
       end if
    case(TID_FIG)
       p2=get_end_of_fig(rpnbuf,k)
       if(p2<0) t=TID_INV
    end select
    
    rpnbuf%old_tid=t
    get_next=t
    rpnbuf%cur_pos=p2
    
  contains
    
    character*1 function next_char(inc)
      integer,intent(in)::inc
      integer kk
      kk=k+inc
      next_char=rpnbuf%expr(kk:kk)
    end function next_char

  end function get_next
  
  subroutine dump_rpnm(rpnc,ent)
    type(t_rpnc),intent(in),target::rpnc
    integer,intent(in)::ent
    type(t_rpnm),pointer::rpnm
    type(t_rpnc) tmprpnc
    integer ptr,len
    if(ent>rpnc%rl%s%n.or.ent<=0.or.ent>size(rpnc%rl%rpnm)&
         .or.get_str_ptr(rpnc%rl%s,ent,ptr,len)/=0) then
       write(*,*) "*** dump_rpnm: no such entry: ",ent
       return
    end if
    rpnm=>rpnc%rl%rpnm(ent)
    write(*,*) "rpnm entry: ",ent
    write(*,*) trim(cpstr(ptr+LEN_SLIST_HDR,len))
    if(associated(rpnm%pnames).and.get_str_ptr(rpnm%pnames,1,ptr,len)==0) then
       write(*,*) trim(cpstr(ptr+LEN_SLIST_HDR,len))
       tmprpnc%que=>rpnm%que
       tmprpnc%vbuf=>rpnm%vbuf
       tmprpnc%p_vbuf=>rpnm%p_vbuf
       tmprpnc%pars=>rpnm%pars
       tmprpnc%answer=>rpnm%answer
       tmprpnc%tmpans=>rpnm%tmpans
       if(associated(rpnm%na)) write(*,*) "number of arguments = ",rpnm%na
       call dump_rpnc(tmprpnc)
    else
       write(*,*) "(empty)"
    end if
  end subroutine dump_rpnm

  subroutine dump_rpnc(rpnc)
    type(t_rpnc),intent(in)::rpnc
    integer i
    complex*16 v
    pointer(pv,v)
    write(*,*) "rpnc dump:\n# tid cid value"
    if(.not.associated(rpnc%que)) then
       write(*,*) "(empty)"
       return
    end if
    do i=1,size(rpnc%que)
       write(*,10) i,rpnc%que(i)%tid,rpnc%que(i)%cid
       select case(rpnc%que(i)%tid)
       case(TID_VAR)
          pv=rpnc%que(i)%cid
          if(pv/=0) then
             write(*,*) v
          else
             write(*,*) "(undef)"
          end if
       case default
          write(*,*)
       end select
    end do
10  format(x,i4,x,i4,x,i9,$)    
    write(*,*) "# vbuf size = ",rpnc%p_vbuf
    if(rpnc%p_vbuf>0) then
       do i=1,rpnc%p_vbuf
          write(*,*) i,loc(rpnc%vbuf(i)),rpnc%vbuf(i)
       end do
    end if
  end subroutine dump_rpnc

#define _EXPR_(i) rpnbuf%expr(get_lo32(rpnbuf%que(i)%p1):get_lo32(rpnbuf%que(i)%p2))
#define _UEXPR_(i) rpnbuf%expr(get_up32(rpnbuf%que(i)%p1):get_up32(rpnbuf%que(i)%p2))

  subroutine set_function(rpnbuf,rpnc)
    type(t_rpnbuf),intent(in)::rpnbuf
    type(t_rpnc),intent(inout),target::rpnc
    type(t_rpnm),pointer::rpnm
    integer i
    integer kf,km,ka
    integer ac,vc,pc,plen
    integer tc
    do i=1,size(rpnc%que)
       if(rpnc%que(i)%tid/=TID_AFNC) cycle
       kf=rpnc%que(i)%cid
       rpnm=>rpnc%rl%rpnm(kf)
       if(associated(rpnm%que).and.size(rpnm%que)>0) deallocate(rpnm%que)
       if(associated(rpnm%vbuf).and.size(rpnm%vbuf)>0) deallocate(rpnm%vbuf)       
       if(.not.associated(rpnm%na)) allocate(rpnm%na)
       if(i==1) then
          km=find_implicit_mul() ! must be found
          if(km==0) stop "*** UNEXPECTED ERROR in set_function"
          ac=km-2 ! number of arguments
       else
          km=i
          ac=i-1

       end if
       tc=size(rpnc%que)-1-(km+1)+1 ! que must end with =
       rpnm%na=ac
       if(tc==0) stop "empty function"
       rpnm%pars=>rpnc%pars
       rpnm%answer=>rpnc%answer
       rpnm%tmpans=>rpnc%tmpans
       allocate(rpnm%que(tc))
       rpnm%que(1:tc)=rpnc%que(km+1:size(rpnc%que)-1)
       call count_var()
       call init_pnames()
       if(vc>0) allocate(rpnm%vbuf(vc))
       call cp_vbuf()
    end do
    
  contains
    
    subroutine init_pnames()
      integer istat
      if(associated(rpnm%pnames)) deallocate(rpnm%pnames)
      allocate(rpnm%pnames)
      plen=plen+get_up32(rpnbuf%que(i)%p2)-get_up32(rpnbuf%que(i)%p1)+1
      rpnm%pnames=init_slist(plen+(pc+1)*LEN_SLIST_HDR)
      istat=add_str(rpnm%pnames,_UEXPR_(i),SC_RO)
      if(istat/=0) stop "hoge"
    end subroutine init_pnames

    integer function find_implicit_mul()
      integer ii
      find_implicit_mul=0
      do ii=1,size(rpnbuf%que)
         if(rpnbuf%que(ii)%tid==TID_BOP4) then
           find_implicit_mul=ii ! start of function definition
            return
         end if
      end do
    end function find_implicit_mul

    subroutine cp_vbuf()
      integer ii,jj
      integer kp
      integer istat
      complex*16 v
      pointer(pv,v)
      if(.not.associated(rpnm%p_vbuf)) allocate(rpnm%p_vbuf)
      rpnm%p_vbuf=0
      do ii=1,tc
         if(rpnm%que(ii)%tid/=TID_VAR) cycle
         jj=ii+km !<<<<<<<<<<<<<<<<
         if(rpnbuf%que(jj)%tid/=TID_FIG) then ! par
            istat=add_str(rpnm%pnames,_EXPR_(jj),SC_RO,ent=kp)            
            if(istat/=0) stop "hego"
            rpnm%que(ii)%tid=rpnbuf%que(jj)%tid
            rpnm%que(ii)%cid=kp
         else
            pv=rpnm%que(ii)%cid
            rpnm%p_vbuf=rpnm%p_vbuf+1
            rpnm%vbuf(rpnm%p_vbuf)=v
            rpnm%que(ii)%tid=TID_FIG
            rpnm%que(ii)%cid=rpnm%p_vbuf
         end if
      end do
    end subroutine cp_vbuf

    subroutine count_var()
      integer ii
      vc=0
      pc=0
      plen=0
      do ii=km,size(rpnc%que)
         select case(rpnc%que(ii)%tid)
         case(TID_VAR) 
            if(rpnbuf%que(ii)%tid/=TID_FIG) then ! par
               plen=plen+rpnbuf%que(ii)%p2-rpnbuf%que(ii)%p1+1
               pc=pc+1
            end if
            vc=vc+1
         case(TID_DPAR)
            vc=vc+1
         case(TID_MAC)
            vc=vc+1
         end select
      end do
    end subroutine count_var

  end subroutine set_function

  subroutine set_macro(rpnbuf,rpnc)
    type(t_rpnbuf),intent(in)::rpnbuf
    type(t_rpnc),intent(inout),target::rpnc
    type(t_rpnm),pointer::rpnm
    integer k,km
    integer i
    integer tc,vc,plen,pc
    integer istat
    do i=1,size(rpnc%que)
       if(rpnc%que(i)%tid/=TID_AMAC) cycle
       k=find_qend() 
       if(k==0) stop "*** UNEXPECTED ERROR in set_macro"
       km=rpnc%que(i)%cid
       rpnm=>rpnc%rl%rpnm(km)
       if(associated(rpnm%que).and.size(rpnm%que)>0) deallocate(rpnm%que)
       if(associated(rpnm%vbuf).and.size(rpnm%vbuf)>0) deallocate(rpnm%vbuf)
       tc=(k-1)-(i+1)+1
       if(tc==0) exit ! empty macro
       rpnm%pars=>rpnc%pars
       rpnm%answer=>rpnc%answer
       rpnm%tmpans=>rpnc%tmpans
       allocate(rpnm%que(tc))
       call count_var()
       call init_pnames()
       rpnm%que(1:tc)=rpnc%que(i+1:i+1+tc-1)
       if(vc>0) allocate(rpnm%vbuf(vc))
       call cp_vbuf()
       rpnc%que(i+1:k)%tid=TID_NOP
       rpnc%que(i)%tid=TID_MAC
    end do
    
  contains

    subroutine init_pnames()
      if(associated(rpnm%pnames)) deallocate(rpnm%pnames)
      allocate(rpnm%pnames)
      plen=plen+get_up32(rpnbuf%que(i)%p2)-get_up32(rpnbuf%que(i)%p1)+1
      rpnm%pnames=init_slist(plen+(pc+1)*LEN_SLIST_HDR)
      istat=add_str(rpnm%pnames,_UEXPR_(i),SC_RO)
      if(istat/=0) stop "hoge" ! <<<<<<<<<<<<<<<<<<
    end subroutine init_pnames

    subroutine cp_vbuf()
      ! Reverts TID_VAR to FIG,PAR,APAR
      ! FIG and PAR will have pointer to vbuf
      ! APAR will have pointer to pars%v
      integer ii,jj
      integer kp
      complex*16 v
      pointer(pv,v)
      if(.not.associated(rpnm%p_vbuf)) allocate(rpnm%p_vbuf)
      rpnm%p_vbuf=0
      do ii=1,tc
         if(rpnm%que(ii)%tid/=TID_VAR) cycle
         jj=ii+i
         if(rpnbuf%que(jj)%tid/=TID_FIG) then ! par
            istat=add_str(rpnm%pnames,_EXPR_(jj),SC_RO,ent=kp)            
            if(istat/=0) stop "hego" ! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
            rpnm%que(ii)%tid=rpnbuf%que(jj)%tid ! <<< TID_PAR or TID_APAR
            rpnm%que(ii)%cid=kp     ! <<<
         else
            pv=rpnm%que(ii)%cid
            rpnm%p_vbuf=rpnm%p_vbuf+1
            rpnm%vbuf(rpnm%p_vbuf)=v
            rpnm%que(ii)%tid=TID_FIG
            rpnm%que(ii)%cid=rpnm%p_vbuf
         end if
      end do
    end subroutine cp_vbuf
    
    subroutine count_var()
      integer ii
      vc=0
      plen=0
      pc=0
      do ii=i+1,k-1
         select case(rpnc%que(ii)%tid)
         case(TID_VAR) 
            if(rpnbuf%que(ii)%tid/=TID_FIG) then ! par
               plen=plen+rpnbuf%que(ii)%p2-rpnbuf%que(ii)%p1+1
               pc=pc+1
               if(rpnbuf%que(ii)%tid/=TID_APAR) vc=vc+1
            else
               vc=vc+1
            end if
         case(TID_MAC)
            vc=vc+1 !<<<<<<<<<<<<<<<<<<<<<<<<<<<<
         end select
      end do
    end subroutine count_var
    
    integer function find_qend()
      integer ii
      find_qend=0
      do ii=i+1,size(rpnc%que)
         if(rpnc%que(ii)%tid==TID_QEND) then
            find_qend=ii
            return
         end if
      end do
    end function find_qend
    
  end subroutine set_macro
      
  integer function build_rpnc(rpnbuf,rpnc,tc)
    type(t_rpnbuf),intent(in)::rpnbuf
    type(t_rpnc),intent(inout),target::rpnc
    integer,intent(in)::tc
    complex*16 v
    real*8 x
    integer istat
    integer i,k
    logical amac,afnc
    amac=.false.
    afnc=.false.
    rpnc%rc=0
    if(associated(rpnc%que).and.size(rpnc%que)>0) deallocate(rpnc%que)
    allocate(rpnc%que(tc))
    rpnc%p_vbuf=0
    do i=1,tc
       istat=0
       select case(rpnbuf%que(i)%tid)
       case(TID_BOP4)
          rpnc%que(i)%tid=TID_OP2
          rpnc%que(i)%cid=OID_MUL
       case(TID_UOP1,TID_UOP2)
          rpnc%que(i)%tid=TID_OP1
          rpnc%que(i)%cid=get_oid1(_EXPR_(i))
       case(TID_BOP1,TID_BOP2,TID_BOP3)
          rpnc%que(i)%tid=TID_OP2
          rpnc%que(i)%cid=get_oid2(_EXPR_(i))
       case(TID_ASN,TID_AOP)
          rpnc%que(i)%tid=TID_ASN
          rpnc%que(i)%cid=get_aid(_EXPR_(i))
       case(TID_MASN)
          rpnc%que(i)%tid=TID_NOP
          rpnc%que(i)%cid=TID_UNDEF
       case(TID_IFNC)
          rpnc%que(i)%tid=TID_IFNC
          rpnc%que(i)%cid=get_fid(_EXPR_(i))
       case(TID_UFNC)
          rpnc%que(i)%tid=TID_UFNC
          rpnc%que(i)%cid=get_ufid(_EXPR_(i))
       case(TID_FIG)
          rpnc%que(i)%tid=TID_VAR
          read(_EXPR_(i),*,iostat=istat) x          
          if(istat/=0) STOP "*** UNEXPECTED ERROR in build_rpn"
          call put_vbuf(complex(x,0.))
       case(TID_PAR) 
          if(rpnc%rl%s%n>0) then
             ! find the macro first
             k=find_str(rpnc%rl%s,_EXPR_(i))
             if(k>0) then
                rpnc%que(i)%tid=TID_MAC
                rpnc%que(i)%cid=k
                cycle
             end if
          end if
          rpnc%que(i)%tid=TID_VAR
          istat=find_par(rpnc%pars,_EXPR_(i),v,k)          
          if(amac.and.istat/=0) then
             ! par may not already exist
             istat=add_par_by_entry(rpnc%pars,_EXPR_(i),k)
             if(istat/=0) then
                write(*,*) "*** add_par_by_entry failed: code = ",istat
                istat=RPNERR_ADDPAR
             end if
          end if
          if(istat==0) then
             call put_vbuf(v)
          else
             write(*,*) "*** No such parameter: "//_EXPR_(i)
             istat=RPNERR_NOPAR
          end if
       case(TID_APAR) ! asign a parameter.
          rpnc%que(i)%tid=TID_VAR
          istat=add_par_by_entry(rpnc%pars,_EXPR_(i),k)
          if(istat==0) then
             rpnc%que(i)%cid=loc(rpnc%pars%v(k)) !<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          else
             write(*,*) "*** add_par_by_entry failed: code = ",istat
             istat=RPNERR_ADDPAR
          end if
       case(TID_QEND)
          amac=.false.
          rpnc%que(i)%tid=TID_QEND
          rpnc%que(i)%cid=0
       case(TID_AFNC)
          afnc=.true.
          rpnc%que(i)%tid=TID_AFNC
          call add_rpnm_entry(SC_FNC)
       case(TID_DPAR)
          rpnc%que(i)%tid=TID_DPAR
          rpnc%que(i)%cid=get_up32(rpnbuf%que(i)%p1)
       case(TID_AMAC)
          amac=.true.
          rpnc%que(i)%tid=TID_AMAC
          call add_rpnm_entry(SC_MAC)
       case default
          STOP "*** UNEXPECTED ERROR in build_rpnc"
       end select
       if(istat/=0) exit
    end do

    build_rpnc=istat

  contains

    subroutine add_rpnm_entry(code)
      integer*1,intent(in)::code
      istat=try_add_str(rpnc%rl%s,_EXPR_(i),code,k)
      if(istat==0) then
         if(k>size(rpnc%rl%rpnm)) then
            write(*,*) "add_rpnm_entry faild: buffer overflow"
            istat=RPNERR_MEMOV
         else
            rpnc%que(i)%cid=k
         end if
      else
         write(*,*) "*** try_add_str failed: code = ",istat
         istat=RPNERR_ADDSTR
      end if
    end subroutine add_rpnm_entry
    
    subroutine put_vbuf(val)
      complex*16,intent(in)::val
      if(rpnc%p_vbuf>=size(rpnc%vbuf)) call inc_vbuf(rpnc,NUM_VBUF_MIN)
      rpnc%p_vbuf=rpnc%p_vbuf+1
      rpnc%vbuf(rpnc%p_vbuf)=val
      rpnc%que(i)%cid=loc(rpnc%vbuf(rpnc%p_vbuf))
    end subroutine put_vbuf

    integer function get_ufid(a)
      character*(*),intent(in)::a
      get_ufid=find_str(rpnc%rl%s,a)
      if(get_ufid==0) STOP "*** UNEXPECTED ERROR in get_ufid"
    end function get_ufid

    integer function get_fid(a)
      character*(*),intent(in)::a
      get_fid=is_int_fnc(a)
      if(get_fid==0) STOP "*** UNEXPECTED ERROR in get_fid"
    end function get_fid

    integer function get_aid(a)
      character*(*),intent(in)::a
      get_aid=AID_NOP ! to avoid warning
      select case(a)
      case("=")
         get_aid=AID_MOV
      case("+=")
         get_aid=AID_INC
      case("-=")
         get_aid=AID_DEC
      case("*=")
         get_aid=AID_MUL
      case("/=")
         get_aid=AID_DIV
      case("^=")
         get_aid=AID_POW
      case default
         STOP "*** UNEXPECTED ERROR in get_aid"
      end select
    end function get_aid

    integer function get_oid1(a)
      character*(*),intent(in)::a
      get_oid1=OID_NOP
      select case(a)
      case("+")
         get_oid1=OID_NOP
      case("-")
         get_oid1=OID_NEG
      case("!")
         get_oid1=OID_FAC
      case default
         STOP "*** UNEXPECTED ERROR in get_oid1"
      end select
    end function get_oid1

    integer function get_oid2(a)
      character*(*),intent(in)::a
      get_oid2=OID_NOP
      select case(a)
      case("+")
         get_oid2=OID_ADD
      case("-")
         get_oid2=OID_SUB
      case("*")
         get_oid2=OID_MUL
      case("/")
         get_oid2=OID_DIV
      case("**","^")
         get_oid2=OID_POW
      case("e")
         get_oid2=OID_EXP
      case default
         STOP "*** UNEXPECTED ERROR in get_oid2"
      end select
    end function get_oid2

  end function build_rpnc

  subroutine dump_rpnbuf(rpnbuf)
    type(t_rpnbuf),intent(in)::rpnbuf
    integer i,p1lo,p2lo,p1up,p2up
    write(*,*) "rpnbuf dump:\n# tid p1 p2 expr"
    if(.not.allocated(rpnbuf%que).or.rpnbuf%p_que<1) then
       write(*,*) "(empty)"
       return
    end if
    do i=1,rpnbuf%p_que
       p1lo=get_lo32(rpnbuf%que(i)%p1)
       p2lo=get_lo32(rpnbuf%que(i)%p2)
       write(*,10) i,rpnbuf%que(i)%tid,p1lo,p2lo
10     format(4(x,i4),x,$)
       if(rpnbuf%que(i)%tid==TID_VAR) then
          write(*,*) rpnbuf%expr(p1lo:p2lo)
       else if(rpnbuf%que(i)%tid==TID_AMAC) then
          p1up=get_up32(rpnbuf%que(i)%p1)
          p2up=get_up32(rpnbuf%que(i)%p2)
          write(*,*) rpnbuf%expr(p1lo:p2lo)//" "//rpnbuf%expr(p1up:p2up)
       else if(p1lo==0) then
          write(*,*) "*"
       else if(rpnbuf%que(i)%tid/=TID_NOP) then
          write(*,*) rpnbuf%expr(p1lo:p2lo)
       else
          write(*,*)
       end if
    end do
  end subroutine dump_rpnbuf

   subroutine rpn_put(rpnbuf,tid,p1,p2)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer,intent(in)::tid,p1,p2
    rpnbuf%p_que=rpnbuf%p_que+1
    rpnbuf%que(rpnbuf%p_que)%tid=tid
    rpnbuf%que(rpnbuf%p_que)%p1=p1
    rpnbuf%que(rpnbuf%p_que)%p2=p2
  end subroutine rpn_put

  subroutine rpn_push(rpnbuf,tid,p1,p2)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer,intent(in)::tid,p1,p2
    rpnbuf%p_buf=rpnbuf%p_buf+1
    rpnbuf%buf(rpnbuf%p_buf)%tid=tid
    rpnbuf%buf(rpnbuf%p_buf)%p1=p1
    rpnbuf%buf(rpnbuf%p_buf)%p2=p2
  end subroutine rpn_push

  subroutine rpn_pop(rpnbuf)
    type(t_rpnbuf),intent(inout)::rpnbuf
    if(rpnbuf%p_buf<0) return
    rpnbuf%p_que=rpnbuf%p_que+1
    rpnbuf%que(rpnbuf%p_que)=rpnbuf%buf(rpnbuf%p_buf)
    rpnbuf%p_buf=rpnbuf%p_buf-1
  end subroutine rpn_pop

  subroutine rpn_pop_all(rpnbuf)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer i
    do i=rpnbuf%p_buf,1,-1
       select case(rpnbuf%buf(i)%tid)
       case(TID_BRA,TID_QSTA,TID_IBRA)
          cycle ! skip unclosed bra
       end select
       rpnbuf%p_que=rpnbuf%p_que+1
       rpnbuf%que(rpnbuf%p_que)=rpnbuf%buf(i)
    end do
    rpnbuf%p_buf=0
  end subroutine rpn_pop_all

  subroutine rpn_try_pop(rpnbuf,tend)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer,intent(in)::tend
    integer tid
    do
       if(rpnbuf%p_buf<=0) exit
       tid=rpnbuf%buf(rpnbuf%p_buf)%tid
       select case(tid)
       case(TID_BRA,TID_QSTA)
          rpnbuf%p_buf=rpnbuf%p_buf-1
          if(tid==tend) exit
       case(TID_IBRA)
          rpnbuf%p_buf=rpnbuf%p_buf-1
       case default
          call rpn_pop(rpnbuf)
       end select
    end do
  end subroutine rpn_try_pop

  subroutine rpn_try_push(rpnbuf,tid,p1,p2)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer,intent(in)::tid,p1,p2
    if(rpnbuf%p_buf==0) then
       call rpn_push(rpnbuf,tid,p1,p2)
    else
       do 
          if(rpnbuf%p_buf<=0) exit
          if(rpnbuf%buf(rpnbuf%p_buf)%tid>=tid) then
             call rpn_pop(rpnbuf)
          else
             exit
          end if
       end do
       call rpn_push(rpnbuf,tid,p1,p2)
    end if
  end subroutine rpn_try_push

  subroutine revert_tid(rpnbuf,tid,p1,p2)
    type(t_rpnbuf),intent(inout)::rpnbuf
    integer,intent(in)::tid
    integer,intent(in),optional::p1,p2
    rpnbuf%que(rpnbuf%p_que)%tid=tid
    if(present(p1)) rpnbuf%que(rpnbuf%p_que)%p1=ior(rpnbuf%que(rpnbuf%p_que)%p1,ishft(p1,16))
    if(present(p2)) rpnbuf%que(rpnbuf%p_que)%p2=ior(rpnbuf%que(rpnbuf%p_que)%p2,ishft(p2,16))
  end subroutine revert_tid

  integer function parse_formula(eq,rpnc,dump)
    character*(*),intent(in)::eq
    type(t_rpnc),intent(inout)::rpnc
    logical,intent(in),optional::dump
    type(t_rpnbuf) rpnbuf
    integer t,told,btold,istat
    integer p1,p2
    integer bc,kc,pc,ac,fc,oc,fnc,qc,cc,apc,amc
    integer tc
    logical amac
    integer pfasn

    amac=.false.
    pfasn=0

    call init_rpnbuf(eq)
    
    btold=TID_NOP
    told=TID_NOP
    istat=0
    bc=0; kc=0; pc=0; ac=0; fc=0; oc=0; fnc=0; qc=0; cc=0
    apc=0; amc=0

    do 
       t=get_next(rpnbuf,p1,p2,rpnc%rl%s)
       select case(t)
       case(TID_FIN)
          if(.not.was_operand()) then
             call print_error()
             istat=RPNERR_PARSER
          else
             if(iand(qc,1)==1) then
                ! close "
                call rpn_try_pop(rpnbuf,TID_QSTA)
                call rpn_put(rpnbuf,TID_QEND,0,0)
             end if
             call rpn_pop_all(rpnbuf)
          end if
          exit
       case(TID_INV,TID_UNDEF)
          call print_error()
          istat=RPNERR_PARSER
       case(TID_PAR)
          pc=pc+1
          select case(told)
          case(TID_FIG,TID_KET)
             call push_implicit_mul()
          end select
          call rpn_put(rpnbuf,t,p1,p2)
       case(TID_FIG)
          fc=fc+1
          if(told==TID_KET) call push_implicit_mul()
          call rpn_put(rpnbuf,t,p1,p2)
       case(TID_BOP1,TID_BOP2,TID_BOP3)
          oc=oc+1
          if(.not.was_operand()) then
             call print_error()
             istat=RPNERR_PARSER        
          else
             call rpn_try_push(rpnbuf,t,p1,p2)
          end if
       case(TID_UOP2)
          oc=oc+1
          select case(told)
          case(TID_FIG,TID_PAR,TID_KET)
             call rpn_try_push(rpnbuf,t,p1,p2)
          case default
             call print_error()
             istat=RPNERR_PARSER
          end select
       case(TID_UOP1)
          oc=oc+1
          call rpn_try_push(rpnbuf,t,p1,p2)
       case(TID_IFNC,TID_UFNC)
          fnc=fnc+1
          select case(told)
          case(TID_KET,TID_FIG)
             call push_implicit_mul()
          end select
          call rpn_try_push(rpnbuf,t,p1,p2)
       case(TID_BRA)
          bc=bc+1
          select case(told)
          case(TID_FIG,TID_PAR,TID_KET)
             call push_implicit_mul()
          end select
          call rpn_push(rpnbuf,t,p1,p2)
       case(TID_KET)
          kc=kc+1
          if(.not.was_operand()) then
             call print_error()
             istat=RPNERR_PARSER
          else
             call rpn_try_pop(rpnbuf,TID_BRA)
          end if
       case(TID_AOP)
          if(check_assignable()) then
             call revert_tid(rpnbuf,TID_APAR)
             call rpn_try_push(rpnbuf,t,p1,p2)
          else
             call print_error()
             istat=RPNERR_PARSER
          end if
       case(TID_ASN)
          ac=ac+1
          if(.not.is_fnc_asn()) then
             if(check_assignable()) then
                apc=apc+1
                call revert_tid(rpnbuf,TID_APAR)
             else
                call print_error()
                istat=RPNERR_PARSER
             end if
          end if
          if(istat==0) then
             call rpn_try_push(rpnbuf,t,p1,p2)
             if(next_chr()/="""") call push_implicit_bra() ! a=b=c=1 is a=(b=(c=(1
          end if
       case(TID_QTN)
          qc=qc+1
          if(and(qc,1)/=0) then
             ! the first "
             if(told/=TID_ASN) then
                call print_error()
                istat=RPNERR_PARSER
             else
                call revert_tid(rpnbuf,TID_AMAC,p1,find_chr(rpnbuf%expr(p1+1:),"""")+p1)
                rpnbuf%buf(rpnbuf%p_buf)%tid=TID_MASN ! it must be TID_ASN
                call rpn_push(rpnbuf,TID_QSTA,p1,p2)
                amac=.true.
             end if
          else
             if(.not.was_operand()) then
                call print_error()
                istat=RPNERR_PARSER
             else
                call rpn_try_pop(rpnbuf,TID_QSTA)
                call rpn_put(rpnbuf,TID_QEND,p1,p2)
             end if
          end if
       case(TID_COMA)
          cc=cc+1
          if(.not.was_operand()) then
             call print_error()
             istat=RPNERR_PARSER
          end if
       case default
          stop "*** UNEXPECTED ERROR in parse_formula"
       end select
       btold=told
       told=t
       if(istat/=0) exit
    end do

    tc=rpnbuf%p_que

    if(kc-bc>0) then
       write(*,*) "*** Too many kets" !<<<<<<<<<<<<<<<<<<<<
       istat=RPNERR_PARSER
    end if

    if(istat==0.and.pfasn/=0) call set_par_dummy()

    if(present(dump).and.dump) call dump_rpnbuf(rpnbuf)

    if(istat==0) istat=build_rpnc(rpnbuf,rpnc,tc)

    if(istat==0.and.amac) call set_macro(rpnbuf,rpnc)

    if(istat==0.and.pfasn/=0) call set_function(rpnbuf,rpnc)

    parse_formula=istat
    
  contains
    
    logical function is_fnc_asn()
      is_fnc_asn=.false.
      if(ac==1.and.bc==1.and.kc==1.and.pc>=1.and.fc==0) then
         if(rpnbuf%buf(rpnbuf%p_buf)%tid==TID_UFNC) then
            rpnbuf%buf(rpnbuf%p_buf)%tid=TID_AFNC
            rpnbuf%buf(rpnbuf%p_buf)%p1=get_i32(rpnbuf%buf(rpnbuf%p_buf)%p1,p1+1)
            rpnbuf%buf(rpnbuf%p_buf)%p2=get_i32(rpnbuf%buf(rpnbuf%p_buf)%p2,rpnbuf%len_expr)
            is_fnc_asn=.true.
         else if(rpnbuf%que(1)%tid==TID_PAR) then
            rpnbuf%que(1)%tid=TID_AFNC
            pc=pc-1 ! <<<<<<<<<<<<<<<<<<<<<
            rpnbuf%que(1)%p1=get_i32(rpnbuf%que(1)%p1,p1+1)
            rpnbuf%que(1)%p2=get_i32(rpnbuf%que(1)%p2,rpnbuf%len_expr)
            is_fnc_asn=.true.
         end if
      end if
      p1=get_i32(p1,1)
      p2=get_i32(p2,p2-1)
      if(.not.is_fnc_asn) return
      pfasn=rpnbuf%p_que
      if(pc-1/=cc.or.set_dummy_par()==0) then
         is_fnc_asn=.false.
         istat=RPNERR_PARSER
      end if
          
    end function is_fnc_asn

    logical function check_assignable()
      check_assignable=.false.
      if(told/=TID_PAR) return
      select case(btold) 
      case(TID_NOP,TID_BRA)
      case(TID_QTN)
         if(iand(qc,1)==0) return ! second "
      case default
         return
      end select
      check_assignable=.true.
    end function check_assignable

    subroutine set_par_dummy()
      integer ii,jj
      integer did
      did=0
      do ii=1,pfasn
         if(rpnbuf%que(ii)%tid==TID_DPAR) then
            did=did+1
            do jj=pfasn+1,rpnbuf%p_que ! <<<<
               if(rpnbuf%que(jj)%tid==TID_PAR) then
                  if(_EXPR_(ii)&
                       ==_EXPR_(jj)) then
                     rpnbuf%que(jj)%tid=TID_DPAR
                     rpnbuf%que(jj)%p1=get_i32(rpnbuf%que(jj)%p1,did) !<<<
                  end if
               end if
            end do
         end if
      end do
    end subroutine set_par_dummy
    
    integer function set_dummy_par()
      integer ii
      integer dummy_count
      dummy_count=0
      do ii=1,rpnbuf%p_que
         if(rpnbuf%que(ii)%tid==TID_PAR) then
            rpnbuf%que(ii)%tid=TID_DPAR
            dummy_count=dummy_count+1
         end if
      end do
      set_dummy_par=dummy_count
    end function set_dummy_par

    integer function find_chr(str,chr)
      character*(*),intent(in)::str
      character*1,intent(in)::chr
      find_chr=index(str,chr)
      if(find_chr==0) find_chr=len_trim(str)
    end function find_chr

    logical function was_operand()
      select case(told)
      case(TID_BOP1,TID_BOP2,TID_BOP3,TID_UOP1,TID_AOP,TID_ASN,TID_BRA,TID_COMA)
         was_operand=.false.
      case default
         was_operand=.true.
      end select
    end function was_operand

    subroutine push_implicit_mul()
      call rpn_try_push(rpnbuf,TID_BOP4,0,0)
      oc=oc+1
    end subroutine push_implicit_mul

    subroutine push_implicit_bra()
      call rpn_push(rpnbuf,TID_IBRA,0,0) 
    end subroutine push_implicit_bra

    subroutine init_rpnbuf(s)
      character*(*),intent(in)::s
      rpnbuf%expr=trim(adjustl(s))
      rpnbuf%len_expr=strip(rpnbuf%expr)
      rpnbuf%cur_pos=0
      rpnbuf%old_pos=0
      rpnbuf%old_tid=0
      rpnbuf%p_buf=0
      rpnbuf%p_que=0
      allocate(rpnbuf%que(rpnbuf%len_expr*2)) ! << at most
      allocate(rpnbuf%buf(rpnbuf%len_expr*2)) ! << at most
!!$      rpnbuf%que(:)%tid=0
!!$      rpnbuf%que(:)%p1=0
!!$      rpnbuf%que(:)%p2=0
    end subroutine init_rpnbuf
        
    subroutine print_error()
      write(*,*) "*** Syntacs Error at: "
      write(*,*) trim(rpnbuf%expr)
      write(*,*) repeat(" ",p1-1)//repeat("^",abs(p2)-p1+1) ! some return with negative p2
    end subroutine print_error

    character*1 function next_chr()
      if(p2<rpnbuf%len_expr) then
         next_chr=rpnbuf%expr(p2+1:p2+1)
      else
         next_chr=""
      end if
    end function next_chr
    
  end function parse_formula
  
end module rpn

program ucalc
  use rpn
  use plist ! for test
  implicit none
  type(t_rpnc) rpnc
  character*16384 str
  integer istat
  logical drpnb
  integer i

  call init_rpnc(rpnc)

  do
     drpnb=.false.

     write(*,10) "> "
10   format(x,a,$)
     read(*,"(a)") str
     if(str=="") cycle
     select case(str)
     case("quit","exit","q")
        exit
     case("pdump","pd")
        call dump_plist(rpnc%pars)
        cycle
     end select
     
     if(str(1:4)=="mac ") then
        read(str(5:),*,iostat=istat) i
        if(istat==0) call dump_rpnm(rpnc,i)
        cycle
     end if

     if(str(1:4)=="del ".and.len_trim(str)>=5) then
        call delete_par(rpnc,str(5:))
        cycle
     end if

     if(str(1:5)=="dump ".and.len_trim(str)>=6) then
        drpnb=.true.
        str(1:4)=""
     end if

     istat=parse_formula(str,rpnc,drpnb)

     if(istat/=0) then
        write(*,*)  "*** parse_formula failed: code = ",istat
        call dump_rpnc(rpnc)
     else
        if(drpnb) then
           write(*,*) "=== Before eval ==="
           call dump_rpnc(rpnc)
        end if

        istat=rpn_eval(rpnc)

        if(istat/=0) then
           write(*,*) "*** rpn_eval failed: code = ",istat
           call dump_rpnc(rpnc)
        else
           if(drpnb) then
              write(*,*) "=== After eval ==="
              call dump_rpnc(rpnc)
           end if
           write(*,*) rpn_ans(rpnc)
        end if
     end if
  end do
  
end program ucalc
