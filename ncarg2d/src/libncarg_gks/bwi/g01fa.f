C
C	$Id: g01fa.f,v 1.3 1994-04-28 23:35:43 fred Exp $
C
      SUBROUTINE G01FA
C
C  Process GKS FILL AREA instruction, send CGM polygon.
C
      include 'g01prm.h'
      include 'gksin.h'
      include 'g01io.h'
      include 'g01ins.h'
      include 'g01wsl.h'
      include 'g01arq.h'
      include 'g01ast.h'
      include 'g01adc.h'
      include 'g01opc.h'
      include 'gksenu.h'
C
      INTEGER  KALL, IPRIM, NBYTES, INDX1, INDX2, IX
      SAVE KALL
C
      DATA  KALL/0/, IPRIM/4/
C
      KALL = KALL+1
      IF  (KALL .EQ. 1) THEN
C
C  If the picture is empty, send the clip indicator and rectangle;
C  set the WSL entry "DISPLAY SURFACE EMPTY" to "NOT EMPTY".
C
        IF (MDEMPT .EQ. GEMPTY) CALL G01CLP(1)
        MDEMPT = GNEMPT
C
C  Process pending attributes.
C
        IF (AGPEND(IPRIM)) THEN
C
C  Some changes are pending.
C
          IF (VALCHG(IVFAIX))  THEN
C
C  GKS FILL AREA INDEX, send CGM fill bundle index.
C
            NBYTES = 1+(MIXFW-1)/8
            CALL GPUTNI (CLFBIX, IDFBIX,  NBYTES,  RERR)
            CALL GPUTPR (MRFAIX, MIXFW,  1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested, clear change flag.
C
            MSFAIX         = MRFAIX
            VALCHG(IVFAIX) = .FALSE.
          END IF
          IF (VALCHG(IVFAIS))  THEN
C
C  GKS FILL AREA INTERIOR STYLE, send CGM INTERIOR STYLE.
C
            NBYTES = 1+(MEFW-1)/8
            CALL GPUTNI (CLINTS, IDINTS, NBYTES,  RERR)
            CALL GPUTPR (MRFAIS, MEFW,  1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested, clear change flag.
C
            MSFAIS         = MRFAIS
            VALCHG(IVFAIS) = .FALSE.
          END IF
          IF (VALCHG(IVFASI))  THEN
C
C  GKS FILL AREA STYLE INDEX, send only the CGM hatch index since
C  ctrans does not support pattern index.
C
            NBYTES = 1+(MIXFW-1)/8
            CALL GPUTNI (CLHAIX, IDHAIX, NBYTES,  RERR)
            CALL GPUTPR (MRFASI, MIXFW, 1, RERR)
C           CALL GPUTNI (CLPTIX, IDPTIX, NBYTES,  RERR)
C           CALL GPUTPR (MRFASI, MIXFW, 1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested, clear change flag.
C
            MSFASI = MRFASI
            VALCHG(IVFASI) = .FALSE.
          END IF
          IF (VALCHG(IVFACI))  THEN
C
C  GKS FILL AREA COLOR INDEX, send CGM fill color.
C
            NBYTES = 1+(MCIXFW-1)/8
            CALL GPUTNI (CLFCLR, IDFCLR, NBYTES, RERR)
            CALL GPUTPR (MRFACI, MCIXFW,  1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested, clear change flag.
C
            MSFACI         = MRFACI
            VALCHG(IVFACI) = .FALSE.
          END IF
          IF (VALCHG(IVPASZ)) THEN
C
C  GKS PATTERN SIZE, send CGM pattern size vectors.
C
            NBYTES = 1+(4*MVDCFW-1)/8
            CALL GPUTNI (CLPTSZ, IDPTSZ, NBYTES, RERR)
            CALL GPUTPR (MRPASZ(1) , MVDCFW,  4, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested, clear change flag.
C
            DO 25 IX=1,4
              MSPASZ(IX) = MRPASZ(IX)
   25       CONTINUE
            VALCHG(IVPASZ) = .FALSE.
          END IF
          IF (VALCHG(IVPARF)) THEN
C
C  GKS PATTERN REFERENCE POINT, send CGM fill reference point.
C
            NBYTES = 1+(2*MVDCFW-1)/8
            CALL GPUTNI (CLFRPT, IDFRPT, NBYTES, RERR)
            CALL GPUTPR (MRPARF(1), MVDCFW, 2, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested, clear change flag.
C
            MSPARF(1) = MRPARF(1)
            MSPARF(2) = MRPARF(2)
            VALCHG(IVPARF) = .FALSE.
          END IF
          IF (ANYASF) THEN
C
C  Some GKS ASF has changed, send CGM ASFs.
C
            CALL G01SAS (IPRIM, RERR)
            IF (RERR.NE.0)  RETURN
          END IF
C
C  Clear aggregate change variable.
C
          AGPEND(IPRIM) = .FALSE.
        END IF
C
C  Treat first call, put out opcode, and points.
C
C
C  Put out opcode (CLASS AND ID) and total LENGTH.
C
        NBYTES = 1+(2*RL1*MVDCFW-1)/8
        CALL GPUTNI (CLPGON, IDPGON, NBYTES, RERR)
        IF (RERR .NE. 0) RETURN
C
C  Put out first points array.
C
C  Truncate points to limits of NDC unit square, convert to VDC,
C  and store in MPXPY.
C
        DO 30 IX=1,RL2
          INDX1 = 2*IX-1
          INDX2 = INDX1+1
          MPXPY(INDX1) = MXOFF+IFIX(FLOAT(MXSCAL)*
     +                   (AMAX1(0.,AMIN1(1.0,RX(IX)))))
          MPXPY(INDX2) = MYOFF+IFIX(FLOAT(MYSCAL)*
     +                   (AMAX1(0.,AMIN1(1.0,RY(IX)))))
   30   CONTINUE
C
C  Send out points.
C
        CALL GPUTPR (MPXPY,  MVDCFW,     2*RL2, RERR)
        IF (RERR .NE. 0) RETURN
C
C  If there is to be no continuation, reset the parameter "KALL".
C
        IF (CONT .EQ. 0) THEN
          KALL = 0
          RETURN
        ENDIF
      ENDIF
C
C  Treat the continuation calls.
C
      IF (KALL .GT. 1) THEN
C
C  Truncate points to limits of the NDC unit square; convert to VDC,
C  and store in MPXPY.
C
        DO 40 IX=1,RL2
          INDX1 = 2*IX-1
          INDX2 = INDX1+1
          MPXPY(INDX1) = MXOFF+IFIX(FLOAT(MXSCAL)*
     -                   (AMAX1(0.,AMIN1(1.0,RX(IX)))))
          MPXPY(INDX2) = MYOFF+IFIX(FLOAT(MYSCAL)*
     -                   (AMAX1(0.,AMIN1(1.0,RY(IX)))))
   40   CONTINUE
        IF (CONT .EQ. 0) THEN
          CALL GPUTPR (MPXPY,  MVDCFW,     2*RL2, RERR)
          IF (RERR .NE. 0) RETURN
          KALL = 0
        ELSE
          CALL GPUTPR (MPXPY,  MVDCFW,     2*RL2, RERR)
          IF (RERR .NE. 0) RETURN
        ENDIF
      ENDIF
      RETURN
C
      END
