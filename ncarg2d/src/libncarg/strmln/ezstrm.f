C
C
      SUBROUTINE EZSTRM(U,V,WORK,IMAX,JMAX)
C
      DIMENSION U(IMAX,JMAX), V(IMAX,JMAX), WORK(1)
C
C The following call is for monitoring library use at NCAR
C
      CALL Q8QST4 ( 'GRAPHX', 'STRMLN', 'EZSTRM', 'VERSION 01')
C
      CALL STRMLN(U,V,WORK,IMAX,IMAX,JMAX,0,IER)
C
      RETURN
      END
