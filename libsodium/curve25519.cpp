#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sodium.h>
#include <boost/assert.hpp>
#include "curve25519_util/curve25519_ref10.h"

void print_256(uint8_t*);
void gen_25519(uint8_t*);

int main()
{
    uint8_t* pk = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(pk);
    printf("************* Reader Private Key **************\n");
    print_256(pk);
    printf("************ Reader Public Key ****************\n");
    uint8_t* pubkey = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_ristretto255_base(pubkey, pk);
    print_256(pubkey);
    printf("\n");
    uint8_t* a = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(a);
    printf("************* Tag Private Key *****************\n");
    print_256(a);
    printf("************* Tag Public Key ******************\n");
    uint8_t* A = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_ristretto255_base(A, a);
    print_256(A);

    printf("\n");
    printf("************* Commitment ****************\n");
    uint8_t* t = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(t);
    uint8_t* T = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_ristretto255_base(T, t);
    print_256(T);

    printf("************* Make Challenge *************\n");
    uint8_t* e = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(e);

    // This uses Blake2b
    uint8_t* c = (uint8_t *) malloc(32 * sizeof(uint8_t));
    uint8_t* shared = (uint8_t *) malloc(64 * sizeof(uint8_t));
    for(int i = 0; i < 32; i++)
    {
        shared[i] = T[i]; // Use tag first to generate challenge.
    }

    for(int i = 0; i < 32; i++)
    {
        shared[i + 32] = e[i]; // Next use reader.
    }
    crypto_hash_sha512(c, shared, sizeof(shared)); //hash the concataneted T and e
    print_256(c);

    printf("*************** d = [B^c] ******************\n");
    uint8_t* d = (uint8_t *) malloc(32 * sizeof(uint8_t));
    BOOST_ASSERT_MSG(crypto_scalarmult_ristretto255(d, t, pubkey) == 0,
                    "Scalar Multiplication of d failed!\n");
    print_256(d);

    printf("************** Respond to challenge *************\n");
    uint8_t* s = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_core_ristretto255_scalar_mul(s, c, t);

    crypto_core_ristretto255_scalar_add(s, s, a);

    crypto_core_ristretto255_scalar_add(s, s, d);

    printf("************* Verification ******************\n");
    uint8_t* d_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    BOOST_ASSERT_MSG(crypto_scalarmult_ristretto255(d_prime, pk, T) == 0,
                     "Scalar Mult Failed, d_prime");
    print_256(d_prime);

    BOOST_ASSERT_MSG(sodium_memcmp(d, d_prime, sizeof(d)) == 0,
                     "d and d_prime aren't equal");
    
    uint8_t* S = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_ristretto255_base(S, s);
    uint8_t* D_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_ristretto255_base(D_prime, d_prime);
    uint8_t* C_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_ristretto255(C_prime, c, T);
    uint8_t* A_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    uint8_t* temp = (uint8_t *) malloc(32 * sizeof(uint8_t));
    
    crypto_core_ristretto255_scalar_sub(temp, S, D_prime);
    print_256(temp);
    crypto_core_ristretto255_scalar_sub(A_prime, temp, C_prime);
    print_256(A_prime);
    print_256(A);
    if (sodium_memcmp(A_prime, A, sizeof(A)) == 0)
    {
        printf("Success!\n");
    }
    
    else
    {
        printf("Failed!\n");
    }
}

void gen_25519(uint8_t* pk)
{
    crypto_core_ristretto255_scalar_random(pk);
}

void print_256(uint8_t* key)
{
    for (int i = 0; i < 32; i++)
    {
        printf("%hhx", key[i]);
    }
    printf("\n");
}
