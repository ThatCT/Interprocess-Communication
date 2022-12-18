/* 
 * Operating Systems
 * Interprocess Communication
 *
 * Service 2 implementation
 *
 */

#include "service2.h"


int service(int data) {
	if (data <= 1)
		return data;

	int n = data * 1023;
	for (int i = 1; i < data && n > 1; ++i) {

		if (n % 2 == 0)
			n = n / 2;
		else
			n = 3 * n + 1;
	}

	return n;
}

