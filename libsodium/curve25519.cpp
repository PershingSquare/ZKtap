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
    uint8_t* fake = (uint8_t *) malloc(32* sizeof(uint8_t));
    gen_25519(fake);
    gen_25519(pk);
    printf("************* Reader Private Key **************\n");
    print_256(pk);
    printf("************ Reader Public Key ****************\n");
    uint8_t* pubkey = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_base(pubkey, pk);
    print_256(pubkey);
    printf("\n");
    uint8_t* a = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(a);
    printf("************* Tag Private Key *****************\n");
    print_256(a);
    printf("************* Tag Public Key ******************\n");
    uint8_t* A = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_base(A, a);
    print_256(A);

    // printf("\n");
    // printf("************* Commitment ****************\n");
    // uint8_t* t = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // gen_25519(t);
    // uint8_t* T = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // crypto_scalarmult_base(T, t);
    // print_256(T);
    //
    // printf("************* Make Challenge *************\n");
    // uint8_t* e = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // gen_25519(e);
    //
    // // This uses Blake2b
    // uint8_t* c = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // uint8_t* shared = (uint8_t *) malloc(64 * sizeof(uint8_t));
    // for(int i = 0; i < 32; i++)
    // {
    //     shared[i] = T[i]; // Use tag first to generate challenge.
    // }
    //
    // for(int i = 0; i < 32; i++)
    // {
    //     shared[i + 32] = e[i]; // Next use reader.
    // }
    // crypto_hash_sha512(c, shared, sizeof(shared)); //hash the concataneted T and e
    // print_256(c);

    printf("*************** d = [B^a] ******************\n");
    uint8_t* d = (uint8_t *) malloc(32 * sizeof(uint8_t));
    uint8_t* f = (uint8_t *) malloc(32 * sizeof(uint8_t));

    BOOST_ASSERT_MSG(crypto_scalarmult(d, a, pubkey) == 0,
                    "Scalar Multiplication of d failed!\n");
    crypto_scalarmult(f, fake, pubkey);
    print_256(d);
    // print_256(f);
    printf("************** Respond to challenge (d'=[A^b]) *************\n");
    // uint8_t* s = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // crypto_core_ed25519_scalar_mul(s, c, t);
    //
    // crypto_core_ed25519_scalar_add(s, s, a);
    //
    // crypto_core_ed25519_scalar_add(s, s, d);

    printf("************* Verification ******************\n");
    uint8_t* d_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    BOOST_ASSERT_MSG(crypto_scalarmult(d_prime, pk, A) == 0,
                     "Scalar Mult Failed, d_prime");
    print_256(d_prime);

    BOOST_ASSERT_MSG(sodium_memcmp(d, d_prime, sizeof(d)) == 0,
                     "d and d_prime aren't equal");

    // uint8_t* S = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // crypto_scalarmult_base(S, s);
    // printf("************** S ***************\n");
    // print_256(S);
    // uint8_t* D_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // crypto_scalarmult_base(D_prime, d_prime);
    // uint8_t* C_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    // crypto_scalarmult(C_prime, c, T);
    // uint8_t* A_prime = (uint8_t *) malloc(32 * sizeof(uint8_t));
    //
    // crypto_core_ed25519_scalar_mul(A_prime, A, D_prime);
    // print_256(A_prime);
    // crypto_core_ed25519_scalar_mul(A_prime, A_prime, C_prime);
    // print_256(A_prime);
    // print_256(A);
    // if (sodium_memcmp(A_prime, S, sizeof(A)) == 0)
    // {
    //     printf("Success!\n");
    // }
    //
    // else
    // {
    //     printf("Failed!\n");
    // }


        uint8_t * msg = (uint8_t *) malloc (32*sizeof(uint8_t));
        uint8_t * mac = (uint8_t *) malloc (32*sizeof(uint8_t));

        randombytes_buf(msg, 32);
        // *msg = sizeof(32)*'c';
        unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
        randombytes_buf(nonce, sizeof(nonce));

        printf("*************** Message ****************\n");
        print_256(msg);

        uint8_t * enc_msg = (uint8_t *) malloc (32*sizeof(uint8_t));
        crypto_aead_aes256gcm_encrypt_detached(enc_msg, mac, NULL,
                                  msg, 32,
                                  NULL, NULL,
                                  NULL, nonce, d_prime);
        printf("*************** Encrypted Message ****************\n");
        print_256(enc_msg);
        uint8_t* dec_msg = (uint8_t *) malloc (32 * sizeof(uint8_t));
        uint8_t* dec_msg_fake = (uint8_t *) malloc (32 * sizeof(uint8_t));

        crypto_aead_aes256gcm_decrypt_detached(dec_msg, NULL,
                                  enc_msg, 32,
                                  mac, NULL,
                                  NULL, nonce, d);
        printf("*************** Decrypted Message ****************\n");
        print_256(dec_msg);
//
//         crypto_aead_aes256gcm_decrypt_detached(dec_msg_fake, NULL,
//                                   enc_msg, 32,
//                                   mac, NULL,
//                                   NULL, nonce, f);
//         printf("*************** Decrypted Message ****************\n");
//         print_256(dec_msg_fake);
//         BOOST_ASSERT_MSG(sodium_memcmp(msg, dec_msg, sizeof(msg)) == 0,
//                          "msg and dec_msg aren't equal");
//
//
}
void gen_25519(uint8_t* pk)
{
    randombytes_buf(pk, 32);
    pk[0] &= 248;
    pk[31] &= 127;
    pk[31] &= 64;
}

void print_256(uint8_t* key)
{
    for (int i = 0; i < 32; i++)
    {
        printf("%hhx", key[i]);
    }
    printf("\n");
}
