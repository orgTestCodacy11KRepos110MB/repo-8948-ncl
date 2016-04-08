;---------------------------------------------------
;  NCL User Guide Example:   NUG_use_Fortran_subroutines.ncl
;
;  KMF 30.10.14
;---------------------------------------------------
if (.not. fileexists("./NUG_fortran_ex01.so")) then
   print("")
   print("You don't have the necessary shared object for this script: ./NUG_fortran_ex01.so")
   print("")
   exit
end if
  

external EX01 "./NUG_fortran_ex01.so"
 
begin
   print("")

;-- Calculate three values of a quadratic equation
   nump = 3
   x    = (/ -1., 0.0, 1.0 /)
   qval = new(nump,float)
   
 ;-- Call the NCL version of your Fortran subroutine.     
   EX01::cquad(-1., 2., 3., nump, x, qval)
   print("Polynomial value = " + qval)     ;-- should be (/0,3,4/)
   
;-- Calculate an arc length.
   xc = (/ 0., 1., 2. /)
   yc = (/ 0., 1., 0. /)
   
;-- Call the NCL version of your Fortran function.
   arclen = EX01::arcln(nump,xc,yc)
   print("Arc length = " + arclen)         ;-- should be 2.82843
   print("")

end