/* 
 * Operating Systems
 * Interprocess Communication
 *
 * Service 1 implementation
 *
 */

#include "service1.h"


int service(int data) {
	if (data <= 1)
		return data;

	int n1 = 0, n2 = 1, n3;
	for (int i = 1; i < data; ++i) {
		n3 = n1 + n2;
		n1 = n2;
		n2 = n3;
	}

	return n3;
}
