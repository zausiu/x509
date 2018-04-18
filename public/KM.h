/*
 * KM.h
 *
 *  Created on: Jun 23, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef ANACONDA_KM_H_
#define ANACONDA_KM_H_

class KM;
extern KM g_km;

class KM
{
public:
	KM();
	virtual ~KM();

	const unsigned char* GetPassphrase();

private:
};

#endif /* ANACONDA_KM_H_ */
