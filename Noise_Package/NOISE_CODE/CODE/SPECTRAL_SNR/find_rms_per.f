cc this program is designed to find SNR for 
cc each CR at any  period like 10 sec by averaging 
cc SNR value between two periods.   
cc the SNR table  at various periods 
cc  are stored  like  COR_CART_WLF.SAC_s_snr.txt.

       
       character*70 infn,outfn
       character*50  fname,fnamesnr
       

ccc    infn includes the list of cross-correlations "COR_*._s"

       read(*,*) per 
       read(*,*) infn
       read(*,*) outfn

       open( 10, file =  infn)
       open( 20, file = outfn)

 100   read(10,*,end = 101) fname
        idindex = index(fname,'_s')
        fnamesnr = fname(1:(idindex+1))//'_snr.txt'

        call findsnr(fnamesnr,per,snrout)

       write(20,*) fname, snrout 
       goto 100
 101   continue
       
       end


c   subrounte used to do snr interpolation

        subroutine findsnr(fname,per,snrout)
        character *50 fname
        real*4 period(50),snr(50)
        real*4 per, snrout

        snrout = 0.

        open(11,file = fname, err = 124 )

        iper = 1

 11     read(11,*,end=111) period(iper),snr(iper)
         iper = iper+1
         goto 11
 111     continue
        
        close(11)
        
        iper = iper-1

        do i = 1,iper-1

        if ( per .ge. period(i) .and. 
     1          per .le. period(i+1)) then
              a = per-period(i)
              b = period(i+1)-per
              snrout = (a*snr(i+1)+b*snr(i))/(a+b)
                 goto 123
         endif
        enddo

        goto 123

 124    write(*,*) 'cannot open  ', fname
 123    return

        end

c +++++++++++++++++++++++++++++++++++++++++++++++++++++++
