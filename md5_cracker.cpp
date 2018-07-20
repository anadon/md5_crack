/******************************************************************************
Copyright (C) Josh Marchall 2018
******************************************************************************/
/******************************************************************************
Crack md5 sums using common ASCII characters
    Copyright (C) 2018  Josh Marshall 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

******************************************************************************/


#include <iostream>
#include <errno.h>
#include <openssl/md5.h>
#include <openssl/bn.h>
#include <string.h>


using namespace std;



//This generates candidate input sequences and hashes them, then checkes that hash
//against the input to match.  If we find a match, print the corrosponding hash input.
bool gen_sequences(const int max_size, const unsigned char match[MD5_DIGEST_LENGTH]){
	unsigned char result[MD5_DIGEST_LENGTH];
	const char min_char = ' ';
	const char max_char = '~'+1;

	//For each of the possible lengths, count through all the numbers that many 
	//bytes can hold
	for(int local_max = 1; local_max < max_size; local_max++){
		unsigned char arr[local_max];
		for(int j = 0; j < local_max; ++j){
			arr[j] = min_char;
		}
		
		int i = 0;
		while(!(i == local_max && arr[local_max-1] == min_char)){
			//hashing and checking logic
			MD5(arr, local_max, result);
			if(0 == memcmp(result, match, MD5_DIGEST_LENGTH)){
				cout << "Found it: ";
				for(int j = 0; j < local_max; ++j){
					cout << arr[j];
				}
				cout << endl;
				return true;
			}

			//increment and carry over logic
			i = 0;
			do{
				arr[i] = arr[i] - min_char;
				arr[i] = arr[i] + 1;
				arr[i] = arr[i] % (max_char - min_char);
				arr[i] = arr[i] + min_char;
				i++;
			}while(i < local_max && arr[i-1] == min_char);
		}
	}

	//didn't find the input to the hash if we get here.
	return false;
}


int main(int argc, char** argv){
	//check number of arguments, but don't bother with validation
	if(argc != 2) return -2;
	if(strlen(argv[1]) != MD5_DIGEST_LENGTH*2) return -1;
	
	//convert the hex encoded md5 sum to the 16 byte equivalent
	BIGNUM *input = BN_new();
	BN_hex2bn(&input, argv[1]);
	unsigned char condensed[MD5_DIGEST_LENGTH];
	BN_bn2bin(input, condensed);
	BN_free(input);

	//start trying to find a hash collision
	gen_sequences(64, condensed);

	return 0;
}
