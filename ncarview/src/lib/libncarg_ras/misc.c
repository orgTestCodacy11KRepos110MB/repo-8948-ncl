/*
 *	$Id: misc.c,v 1.7 1992-03-27 20:59:59 clyne Exp $
 */
/***********************************************************************
*                                                                      *
*                          Copyright (C)  1991                         *
*            University Corporation for Atmospheric Research           *
*                          All Rights Reserved                         *
*                                                                      *
*                                                                      *
***********************************************************************/
/*	File:	misc.c
 *
 *	Author: Don Middleton
 *		National Center for Atmospheric Research
 *		Scientific Visualization Group
 *		Boulder, Colorado 80307
 *
 *	Date:	1/31/91
 *
 *	Description:
 *		This file contains miscellaneous functions that
 *		are used by other members of the "libraster"
 *		package.
 */
#include <ncarv.h>
#include "ncarg_ras.h"

extern char	*ProgramName;


static unsigned long	swaptest = 1;

int
read_swap(fp, nb, buf, swapflag)
	FILE	*fp;
	int	nb;
	char	*buf;
	int	swapflag;
{
	int	status;

	status = fread( (char *) buf, 1, nb, fp);
	if (status != nb) return(RAS_EOF);

	if (swapflag && *(char *) &swaptest)
		_swaplong((char *) buf, (unsigned) nb);
	
	return(RAS_OK);
}

int
read_decode(fd, nbytes)
	int	fd;
	int	nbytes;
{
	int		status;
	unsigned char	buf[4];

	if (nbytes > 4) {
		(void) RasterSetError(RAS_E_INTERNAL_PROGRAMMING);
		return(RAS_ERROR);
	}

	status = read(fd, (char *) buf, nbytes);
	if (status != nbytes) {
		(void) RasterSetError(RAS_E_SYSTEM);
		return(RAS_ERROR);
	}
	else {
		return(char_decode(buf, nbytes));
	}
}

int
char_decode(buf, nbytes)
	unsigned char	*buf;
	int		nbytes;
{
	int		i;
	unsigned int	result = 0;

	for(i=0; i<nbytes; i++)
		result = (result << 8) | buf[i];
	
	return(result);
}


/* Swiped from John Clyne */

_swapshort (bp, n)
    register char *bp;
    register unsigned n;
{
    register char c;
    register char *ep = bp + n;

    while (bp < ep) {
	c = *bp;
	*bp = *(bp + 1);
	bp++;
	*bp++ = c;
    }
}

_swaplong (bp, n)
    register char *bp;
    register unsigned n;
{
    register char c;
    register char *ep = bp + n;
    register char *sp;

    while (bp < ep) {
	sp = bp + 3;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	sp = bp + 1;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	bp += 2;
    }
}

/*
 *	ImageCount_()
 *
 *	Return the number of images in a file. Return -1 on error
 */
ImageCount_(name, format)
	char	*name;
	char	*format;
{
	Raster	*ras, *RasterOpen();
	int	count;
	int	rc;

	if ((ras = RasterOpen(name, format)) == (Raster *) NULL) {
		ESprintf(
			E_UNKNOWN, "RasterOpen(%s,%s) : %s",
			name,format,RasterGetError()
			);
		return(-1);
	}

	count = 0;
	while ((rc = RasterRead(ras)) == RAS_OK) {
		count++;
	}
	if (rc == RAS_ERROR) {
		ESprintf(E_UNKNOWN, "RasterRead() : %s", RasterGetError());
		count = -1;
	}

	RasterClose(ras);
	return(count);
}
