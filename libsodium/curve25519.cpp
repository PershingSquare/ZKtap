#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sodium.h>

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

    printf("\n");
    printf("************* Commitment ****************\n");
    uint8_t* t = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(t);
    uint8_t* T = (uint8_t *) malloc(32 * sizeof(uint8_t));
    crypto_scalarmult_base(T, t);
    print_256(T);
    
    printf("************* Make Challenge *************\n");
    uint8_t* e = (uint8_t *) malloc(32 * sizeof(uint8_t));
    gen_25519(t);
    
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
    crypto_hash_sha512(c, shared, sizeof(shared));
    print_256(c);

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
