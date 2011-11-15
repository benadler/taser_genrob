#include <math.h>
#include <unistd.h>

#include "./laserFeeder.h"
#include "./genBase.h"

#include "math/vec.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>


CLASERFEEDER::CLASERFEEDER (CGENBASE *genBase, const bool simulate) {
    struct sockaddr_in sa;
    int on = 1, i;
    struct ip_mreqn mreq;
    
    GTL ("CLASERFEEDER::CLASERFEEDER()");
    _genBase = genBase;   // for callbacks
    
    _numLaser = 2;
    
    if (!(_alarm_level = (unsigned *) malloc(sizeof(unsigned)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }

    if (!(_platform_valid = (unsigned *) malloc(sizeof(unsigned)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }

    if (!(_x0 = (double *) malloc(sizeof(double)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }

    if (!(_y0 = (double *) malloc(sizeof(double)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }

    if (!(_a0 = (double *) malloc(sizeof(double)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }

    if (!(scantime = (time_t *) malloc(sizeof(time_t)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }
    if (!(marktime = (time_t *) malloc(sizeof(time_t)*_numLaser))) {
	fprintf(stderr, "Error: malloc() failed: %s\n", strerror(errno));
	return;
    }
    
    _scanner = new CRADIALSCAN * [_numLaser];
    _platform = new CRADIALSCANMATCHED * [_numLaser];
    for (i = 0; i < _numLaser; i++) {
	_platform_valid[i] = 0;
	_scanner[i] = new CRADIALSCAN();
	_platform[i] = new CRADIALSCANMATCHED();
	scantime[i] = 1;
	marktime[i] = 1;
	_alarm_level[i] = 2;
    }

    _x0[0] = 0.336;
    _x0[1] = -0.336;
    _y0[0] = 0.0;
    _y0[1] = 0.0;
    _a0[0] = 0.0;
    _a0[1] = M_PI;
    
    buf = NULL;
    buflen = 0;
    
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
	fprintf(stderr, "Error: socket() failed: %s\n", strerror(errno));
	GTLFATAL(("Error: socket() failed: %s\n", strerror(errno)));
	return;
    }
    
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(2222);
    sa.sin_addr.s_addr = INADDR_ANY;
    
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    if (bind(fd, (struct sockaddr*) &sa, sizeof(sa)) < 0) {
	fprintf(stderr, "Error: bind() failed: %s\n", strerror(errno));
	close(fd);
	return;
    }
    
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.23");
    mreq.imr_address.s_addr = INADDR_ANY;
    mreq.imr_ifindex = if_nametoindex("eth0");
    
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		   &mreq, sizeof(mreq)) < 0) {
	fprintf(stderr, "Error: setsockopt() failed: %s\n", strerror(errno));
	close(fd);
	return;
    }
    
    
    
    Start();
}

///////////////////////////////////////////////////////////////////////////////

CLASERFEEDER::~CLASERFEEDER (void) {
    GTL ("CLASERFEEDER::~CLASERFEEDER()");
    
    _numLaser = 0;
    
    Stop();
}


///////////////////////////////////////////////////////////////////////////////

void CLASERFEEDER::raw2radial(unsigned char *const buf, int idx) {

    unsigned char *data;
    int cnt;

    if ((buf[5] != 105) || (buf[6] != 65 && buf[6] != 69)) {
	fprintf(stderr, "scan data weirdness, ignoring %02x%02x\n", 
		buf[5], buf[6]);
	return;
    }

    data = buf+7;
    cnt = 0;
    for (int i = -180; i < 181; i++) {
	unsigned short dist = *(data++);
	dist |= (*(data++)&0x1f)<<8;
	if (dist <= 0x1ff7) {
	    _scanner[idx]->_scanDist[cnt] = (float) dist/1000.0; // in Meter
	    _scanner[idx]->_scanAngle[cnt] = i*0.5*M_PI/180.0;
	    cnt++;
	}
    }

    struct timeval now;
    if (gettimeofday(&now, NULL)) {
	fprintf(stderr, "Error: gettimeofday() failed: %s\n", strerror(errno));
    }
    scantime[idx] = now.tv_sec;

    _scanner[idx]->_numScans = cnt;
    _platform_valid[idx] = 0;
}

void CLASERFEEDER::marks2matched(unsigned char *const buf, 
		   size_t l,
		   int idx) {
    unsigned char *data;
    if (l<4 || (size_t)((buf[2]|(buf[3]<<8))+4) != l) {
	fprintf(stderr, "mark data weirdness, ignoring\n");
	return;
    }
    int nummarks = ((buf[2]|(buf[3]<<8))-2)/4;

    data = buf+5;
    for (int i = 0; i < nummarks; i++) {
	unsigned short dist = *(data++);
	dist |= (*(data++))<<8; // in mm
	unsigned short angle = *(data++);
	angle |= (*(data++))<<8; // in 120.tel Grad


	double rad = (angle/120.0-90.0)*M_PI/180.0;
	double x = cos(rad) * dist/1000.0; // in Meter
	double y = sin(rad) * dist/1000.0;

	_scanner[idx]->_markDist[i] = sqrt(x*x + y*y);
	_scanner[idx]->_markAngle[i] = atan2(y, x);

	rad = rad + _a0[idx];
	x = _x0[idx] + cos(rad) * dist/1000.0; // in Meter
	y = _y0[idx] + sin(rad) * dist/1000.0;

	_platform[idx]->_markDist[i] = sqrt(x*x + y*y);
	_platform[idx]->_markAngle[i] = atan2(y, x);
    }

    struct timeval now;
    if (gettimeofday(&now, NULL)) {
	fprintf(stderr, "Error: gettimeofday() failed: %s\n", strerror(errno));
    }
    marktime[idx] = now.tv_sec;

    _scanner[idx]->_numMarks = nummarks;
    _platform[idx]->_numMarks = nummarks;

}

inline void dumplaser(const CVEC &mark, int *matched, int numMarks) {

    printf("numMarks: %d\n", numMarks);
    int i2 = 0;
    for (int i=0; i < numMarks; i++) {
	printf("%d: %f %f - %d\n", i, mark[i2++], mark[i2++], matched[i]);
    }
}

void CLASERFEEDER::Fxn (void) {
    GTL ("CLASERFEEDER::Fxn()");
    ConfirmRunning();
    
    GTLPRINT (("pid=%i\n", getpid ()));
    
    ScheduleRealtime (20);
    nice (-14);   // for `top' statistics only, not used in rt-scheduling
    
    while (!_stopRequested) {
	
	
	fd_set rfds;
	size_t l; // how many bytes are going to be read by recv()
	ssize_t rl;
	
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	
	if (select(fd+1, &rfds, NULL, NULL, NULL) < 0) {
	    fprintf(stderr, "Error: select() failed: %s\n", strerror(errno));
	    close(fd);
	    return;
	}
	
	if (ioctl(fd, FIONREAD, &l) < 0) {
	    fprintf(stderr, "Error: ioctl() failed: %s\n", strerror(errno));
	    close(fd);
	    return;
	}
	if (buflen < l)
	    if (!(buf = (unsigned char*) realloc(buf, buflen = l+1))) {
		fprintf(stderr, "Error: out of memory\n");
		close(fd);
		return;
	    }
	
	if ((rl = recv(fd, buf, buflen, 0)) < 0) {
	    fprintf(stderr, "Error: recv() failed: %s\n", strerror(errno));
	    close(fd);
	    return;
	}
	
	assert(l == (unsigned)rl);
	buf[rl] = 0;
	
	
	//////////////////////////////////////
	// sammeln von zwei Laserscans
	if (buf[0] != 0x02) {
	    GTLPRINT(("STX not found\n"));
	    continue;
	}

	switch(buf[4]) {
	    case 0xb0: // normale Scandaten
		if (l == 732)
		    raw2radial(buf, ((buf[1]-0x80) ? 1:0));
		continue;
		break;
	    case 0xb3: // Marken und Alarm_level
		marks2matched(buf, l, ((buf[1]-0x90) ? 1:0));
		_alarm_level[((buf[1]-0x90) ? 1:0)] = buf[(buf[2]|(buf[3]<<8))+4-1];
		break;
	    default:
		continue;
	}
	

	// Marken zum Lokalisieren senden
	if (!(buf[1]&0x01)) {
	    int numMarks = 0;
	    
	    for (int l=0; l<_numLaser; l++)	{
		numMarks += _platform[l]->_numMarks;
		
	    }
	    GTLPRINT (("got %i marks\n", numMarks));
	    
	    int matched[numMarks];
	    
	    for (int i=0; i<numMarks; i++) {
		matched[i] = -1;
	    }
	    
	    if (numMarks >= 1) {
		int idx = 0;
		
		CVEC mark (2 * numMarks);
		
		for (int l=0; l<_numLaser; l++) {
		    for (int m=0; m<_platform[l]->_numMarks; m++) {
			mark[idx++] = _platform[l]->_markDist[m] * 1000.0; // mm in m
			mark[idx++] = _platform[l]->_markAngle[m];
		    }
		}

		GTLPRINT (("Feed results to localization.\n"));
		
		// finally, feed the results to the localization
		_genBase->UpdateLaser(mark, matched);
	    }

	    int idx = 0;
	    
	    for (int l=0; l<_numLaser; l++) {
		for (int m=0; m<_platform[l]->_numMarks; m++) {
		    _platform[l]->_markIdx[m] = matched[idx++];
		}
	    }
	}
    }
}

///////////////////////////////////////////////////////////////////////////////

void CLASERFEEDER::updatePlatform(const int idx) {
	// converts values from _scanner to _platform
    if (_platform_valid[idx]) return;

    if ((_platform[idx]->_maxScans != _scanner[idx]->_maxScans) || 
	(_platform[idx]->_maxMarks != _scanner[idx]->_maxMarks)) {
	fprintf(stderr, "scan sizes don't fit\n");
	abort();
    }
    for (int k=0; k < _scanner[idx]->_numScans; k++) {
	double rad = (_scanner[idx]->_scanAngle[k]+_a0[idx]);
	double x = _x0[idx] + cos(rad) * _scanner[idx]->_scanDist[k];
	double y = _y0[idx] + sin(rad) * _scanner[idx]->_scanDist[k];
	_platform[idx]->_scanAngle[k] = atan2(y, x);
	_platform[idx]->_scanDist[k]  = sqrt(x*x + y*y);
    }
    _platform[idx]->_numScans = _scanner[idx]->_numScans;
    _platform_valid[idx] = 1;

}

///////////////////////////////////////////////////////////////////////////////

int CLASERFEEDER::GetClosestObstacleDistance (const int idx,
					      double &distance) {
    GTL (("CLASERFEEDER::GetClosestObstacleDistance()"));
    
    //printf("GetClosestObstacleDistance(%d) request alarm=%d\n", idx, _alarm_level[idx]);
    
    if ((idx < 0) || (idx >= _numLaser)) {
	GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
	distance = 0.3;
	return -1;
    }

    struct timeval now;
    if (gettimeofday(&now, NULL)) {
	fprintf(stderr, "Error: gettimeofday() failed: %s\n", strerror(errno));
    }
    
    if (now.tv_sec - marktime[idx] < 2) {
	if (!_alarm_level[idx]) {
	    distance = 1.01;
	    return 0;
	}
	if (_alarm_level[idx] == 2) {
	    distance = 0.3;
	    return 0;
	}
    } else {
	//printf("markdata too old in getdistance\n");
    }
    
    if (now.tv_sec - scantime[idx] > 1) {
	distance = 0.3;
	//printf("scan too old in getdistance\n");
	return 0;
    }
    
    updatePlatform(idx);
    
    double mindist = 1.01;
    for (int k=0; k < _platform[idx]->_numScans; k++) {
	if (_platform[idx]->_scanDist[k] < mindist) {
	    mindist = _platform[idx]->_scanDist[k];
	}
    }
    //printf("mindist = %f\n", mindist);
    distance = mindist;
    
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

int CLASERFEEDER::GetLaserScanScanner (const int idx,
				       CRADIALSCAN &scan)
{
    GTL (("CLASERFEEDER::GetLaserScanScanner()"));

    //printf("ScanScanner: %u\n", idx);

    if ((idx < 0) || (idx >= _numLaser)) {
	GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
	return -1;
    }

    struct timeval now;
    if (gettimeofday(&now, NULL)) {
	fprintf(stderr, "Error: gettimeofday() failed: %s\n", strerror(errno));
    }

    if (now.tv_sec - scantime[idx] > 1) {
	scan._numScans = 0;
	scan._numMarks = 0;
	return 0;
    }
    
    scan = *_scanner[idx];
    return 0;
}


int CLASERFEEDER::GetLaserScanPlatform (const int idx,
					CRADIALSCAN &scan) {
    GTL (("CLASERFEEDER::GetLaserScanPlatform()"));

    //printf("ScanPlatform: %u\n", idx);

    if ((idx < 0) || (idx >= _numLaser)) {
	GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
	return -1;
    }

    struct timeval now;
    if (gettimeofday(&now, NULL)) {
	fprintf(stderr, "Error: gettimeofday() failed: %s\n", strerror(errno));
    }

    if (now.tv_sec - scantime[idx] > 1) {
	scan._numScans = 0;
	scan._numMarks = 0;
	return 0;
    }

    updatePlatform(idx);
    scan = *_platform[idx];
    return 0;
}


int CLASERFEEDER::GetLaserScanPlatformMatched (const int idx,
					       CRADIALSCANMATCHED &scan) {
    GTL (("CLASERFEEDER::GetLaserScanPlatformMatched()"));

    //printf("ScanPlatformMatched: %u\n", idx);
    
    if ((idx < 0) || (idx >= _numLaser)) {
	GTLPRINT (("illegal laser index %i (0..%i)\n", idx, _numLaser-1));
	return -1;
    }

    struct timeval now;
    if (gettimeofday(&now, NULL)) {
	fprintf(stderr, "Error: gettimeofday() failed: %s\n", strerror(errno));
    }

    if (now.tv_sec - scantime[idx] > 1) {
	scan._numScans = 0;
	scan._numMarks = 0;
	return 0;
    }

    updatePlatform(idx);
    scan = *_platform[idx];
    return 0;
}
