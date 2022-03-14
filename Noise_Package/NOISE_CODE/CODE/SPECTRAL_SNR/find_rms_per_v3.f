cc this program is designed to find SNR for 
cc each CR at any  period like 10 sec by averaging 
cc SNR value between two periods.   
cc the SNR table  at various periods 
cc  are stored  like  COR_CART_WLF.SAC_s_snr.txt.

cc this v3 differs  from find_rms_per_V2.f
cc this version output evlo,evla,stlo,stla, az,baz following the SNR


       real*4 tdata(100)
       character*70 infn,outfn
       character*50  fname,fssnr,fpsnr,fnsnr,fnamesac
       real*4 ssnrout,nsnrout,psnrout

ccc    infn includes the list of cross-correlations "COR_*._s"

       read(*,*) per 
       read(*,*)  infn
       read(*,*) outfn

       open( 10, file =  infn)
       open( 20, file = outfn)

 100   read(10,*,end = 101) fname
        idindex = index(fname,'_s')
        fnamesac = fname(1:idindex-1)
         
        fssnr = fname(1:(idindex+1))//'_snr.txt'
        fnsnr = fname(1:(idindex))//'n_snr.txt'
        fpsnr = fname(1:(idindex))//'p_snr.txt'
        call findsnr(fssnr,per,ssnrout)
        call findsnr(fnsnr,per,nsnrout)
        call findsnr(fpsnr,per,psnrout)

          call rsac1(fnamesac,tdata,nstapts,beg,
     1       delt,maxpts,nerr)
          call getfhv('DIST',dist,nerr)
          call getfhv('STLA',stla,nerr)
          call getfhv('STLO',stlo,nerr)
          call getfhv('EVLA',evla,nerr)          
          call getfhv('EVLO',evlo,nerr)
          call getfhv('AZ', az, nerr)
          call getfhv('BAZ',baz, nerr)


       write(20,15) fname(1:idindex-1), psnrout,nsnrout,ssnrout,
     1  evlo,evla,stlo,stla, dist, az, baz     
       goto 100
 101   continue
 15     format(A20,3x,3f8.2,4f9.2, f8.0, 2f6.0)
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
