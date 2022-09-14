#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <gmp.h>

bool validate_prime(mpz_t n)
{
    return mpz_probab_prime_p(n, 15) != 0;
}

void get_number(char* prompt, mpz_t out)
{
    printf("%s: ", prompt);
    
    char inputStr[1024];
    scanf("%1023s", inputStr);

    int flag = mpz_set_str(out, inputStr, 10);
    if (flag != 0)
    {
        printf("\nInput not a number.\n");
        exit(1);
    };
}


int main()
{
    mpz_t g;
    mpz_init_set_ui(g, 2);

    mpz_t p;
    mpz_init(p);
    get_number("Enter the prime p", p);

    if (!validate_prime(p))
    {
        printf("The entered number is not a prime!\n");
        return 1;
    }

    mpz_t q;
    mpz_init(q);
    mpz_sub_ui(q, p, 1);
    mpz_fdiv_q_2exp(q, q, 1);

    if (!validate_prime(q))
    {
        printf("The entered prime is not a safe prime, that is, (p - 1) / 2 is not prime!");
        return 1;
    }

    mpz_clear(q);

    mpz_t s;
    mpz_init(s);
    get_number("Enter the secret key exponent s", s);

    mpz_t k;
    mpz_init(k);
    mpz_powm(k, g, s, p);

    mpz_t m;
    mpz_init(m);
    get_number("Enter the message m (an integer)", m);

    mpz_t r;
    mpz_init(r);
    get_number("Enter the encryption exponent", r);

    mpz_t c1, c2;
    mpz_init(c1);
    mpz_powm(c1, g, r, p);

    mpz_init(c2);
    mpz_powm(c2, k, r, p);
    mpz_mul(c2, c2, m);
    mpz_mod(c2, c2, p);

    mpz_t c1_inv;
    mpz_init(c1_inv);
    mpz_powm(c1_inv, c1, s, p);

    // mod. inverse p of x is x^(p-2) mod p when p is prime
    mpz_t tmp;
    mpz_init_set(tmp, p);
    mpz_sub_ui(tmp, p, 2);
    mpz_powm(c1_inv, c1_inv, tmp, p);

    // calculate the result of the decryption
    mpz_mul(tmp, c2, c1_inv);
    mpz_mod(tmp, tmp, p);

    printf("\n");

    printf("Public key is: k = ");
    mpz_out_str(stdout, 10, k);
    printf("\n");

    printf("Cryptogram is c = (c1, c2) = (");
    mpz_out_str(stdout, 10, c1);
    printf(", ");
    mpz_out_str(stdout, 10, c2);
    printf(")\n");

    printf("Inverse modulo ");
    mpz_out_str(stdout, 10, p);
    printf(" of ");
    mpz_out_str(stdout, 10, c1);
    printf("^");
    mpz_out_str(stdout, 10, s);
    printf(" is ");
    mpz_out_str(stdout, 10, c1_inv);
    printf("\n\n");

    printf("Encrypted and decrypted messages match: %s.", mpz_cmp(tmp, m) == 0 ? "true" : "false");

    return 0;
}
