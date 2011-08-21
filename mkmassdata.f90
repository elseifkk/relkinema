program mkmassdata
  implicit none
  integer,parameter::MAXPATHLEN=1024
  integer,parameter::narg=2
  integer,parameter::len_element=8
  integer m
  character(MAXPATHLEN) fin,foutd,fout
  character*32 stra
  character*1 NL
  integer NsubZ,N,Z,A
  character*3 Decay
  character*8 El
  real*8 MassExcess
  character*13 me
  logical begin
  integer ounit
  integer istat,k

  m=command_argument_count()
  if(m<narg) then
     call usage()
     call claimer("*** Too few arguments!")
     call exit(1)
  end if

  call get_command_argument(1,fin)
  call get_command_argument(2,foutd)

  open(unit=1,file=fin,status="old",iostat=istat)
  if(istat/=0) call claimer("*** Cannot open "//trim(fin))

  begin=.false.
  ounit=0
  do
     read(1,10,iostat=istat) NL,NsubZ,N,Z,A,EL,Decay,me
     if(istat<0) exit
     k=index(me,"#")
     if(k/=0) me(k:k)="."
     read(me,*,iostat=istat) MassExcess
     if(istat/=0) cycle
     EL=adjustl(EL)
     if(.not.begin) then
        if(NsubZ==1.and.N==1.and.Z==0.and.A==1) then
           begin=.true.
        else
           cycle
        end if
     end if
     write(stra,*) A
     fout=trim(foutd)//"/"//"A"//trim(adjustl(stra))//".dat"
     if(NL=="0") then
        if(ounit/=0) close(ounit)
        ounit=2
        open(unit=ounit,file=fout,status="replace",iostat=istat)
        if(istat/=0) call claimer("*** Cannot open "//trim(fout))
     end if
     write(2,20) EL,MassExcess
20   format(a,1x,f13.5)
10   format(a1,i3,i5,i5,i5,1x,a3,a4,1x,a13)
  end do
  if(ounit/=0) close(ounit)
contains
  subroutine usage()
    write(*,*) "Usage: mkmassdata input_data output_directory."
    write(*,*) "    input_data is Atomic Mass Evaluation file, available at"
    write(*,*) "    http://www.nndc.bnl.gov/amdc/web/masseval.html"
  end subroutine usage
  subroutine claimer(mess,code)
    character*(*),intent(in)::mess
    integer,intent(in),optional::code
    integer rc
    write(*,*) trim(mess)
    rc=1
    if(present(code)) rc=code
    call exit(rc)
  end subroutine claimer
end program mkmassdata
