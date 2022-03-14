cc this program is designed to find SNR for 
cc each CR at any  period like 10 sec by averaging 
cc SNR value between two periods.   
cc the SNR table  at various periods 
cc  are stored  like  COR_CART_WLF.SAC_s_snr.txt.

cc this v2 differs  from find_rms_per.f
cc this version output the SNR of three components for each CR
cc while  find_rms_per.f only output SNR of one single component
       
       character*70 infn,outfn
       character*50  fname,fssnr,fpsnr,fnsnr
       real*4 ssnrout,nsnrout,psnrout

ccc    infn includes the list of cross-correlations "COR_*._s"

       read(*,*) per 
       read(*,*)  infn
       read(*,*) outfn

       open( 10, file =  infn)
       open( 20, file = outfn)

 100   read(10,*,end = 101) fname
        idindex = index(fname,'SAC')
         
        fssnr = fname(1:(idindex+2))//'_snr.txt'
c        fnsnr = fname(1:(idindex))//'n_snr.txt'
c        fpsnr = fname(1:(idindex))//'p_snr.txt'

        call findsnr(fssnr,per,ssnrout)
c        call findsnr(fnsnr,per,nsnrout)
c        call findsnr(fpsnr,per,psnrout)

       write(20,15) fname(1:idindex+2), ssnrout,ssnrout,ssnrout 
       goto 100
 101   continue
 15     format(A40,3x,3f8.2)
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
