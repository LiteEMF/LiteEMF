/* vim: set sw=4 ts=4:
 * Author: Liu DongMiao <liudongmiao@gmail.com>
 * Created  : Wed 18 Apr 2018 09:56:03 AM CST
 * Modified : Thu 26 Apr 2018 03:54:31 PM CST
 */


#include "adb_auth_sign.h"

#if ADB_AUTH_SIGN_ENABLE
#include "mbedtls/bignum.h"


#define load_static(x, y) do {\
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&x, (const unsigned char *)y, sizeof(y) - 1)); \
} while (0)

#ifdef DEBUG

static void adb_auth_dump(const char *p, mbedtls_mpi m) {
    unsigned char *c;

#ifdef __ARMCC_VERSION
#define debug(x, y) printf(x, y)
#else
#define debug(x, y) fprintf(stderr, x, y)
#endif
    debug("%s: ", p);
    c = (unsigned char *) m.p + (m.n * sizeof(mbedtls_mpi_uint)) - 1;
    while (c >= (unsigned char *) m.p) {
        if (*c-- != 0) {
            break;
        }
    }
    while (c >= (unsigned char *) m.p) {
        debug("%02X", *c--);
    }
    debug("%s", "\n");
}

#else

#define adb_auth_dump(x, y) do {} while (0)

#endif


int adb_auth_sign(const unsigned char *token, size_t token_size, unsigned char *sig) {
    int ret;
    mbedtls_mpi p, q, dpq;
    mbedtls_mpi m, m1, m2;

    if (token_size != SHA1_SIZE) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }

    mbedtls_mpi_init(&p);
    mbedtls_mpi_init(&q);
    mbedtls_mpi_init(&dpq);
    mbedtls_mpi_init(&m);
    mbedtls_mpi_init(&m1);
    mbedtls_mpi_init(&m2);

    // load p, q
    load_static(p, KEY_P);
    load_static(q, KEY_Q);
    adb_auth_dump("p", p);
    adb_auth_dump("q", q);

    // load m1, m2
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&m, token, token_size));
    adb_auth_dump("c", m);
    load_static(m1, KEY_CP);
    load_static(m2, KEY_CQ);
    mbedtls_mpi_add_mpi(&m1, &m1, &m);
    mbedtls_mpi_add_mpi(&m2, &m2, &m);
    adb_auth_dump("m1", m1);
    adb_auth_dump("m2", m2);

    load_static(dpq, KEY_DP);
    load_static(m, KEY_RP);
    adb_auth_dump("dp", dpq);
    adb_auth_dump("rp", m);

    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&m1, &m1, &dpq, &p, &m));
    adb_auth_dump("m1 = m1 ^ dp % p", m1);

    load_static(dpq, KEY_DQ);
    load_static(m, KEY_RQ);
    adb_auth_dump("dq", dpq);
    adb_auth_dump("rq", m);

    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&m2, &m2, &dpq, &q, &m));
    adb_auth_dump("m2 = m2 ^ dq % q", m2);

    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&m1, &m1, &m2));
    if (m1.s < 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&m1, &m1, &p));
    }
    adb_auth_dump("m1 - m2", m1);

    load_static(dpq, KEY_QP);
    adb_auth_dump("qp", dpq);

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&m1, &dpq, &m1));
    adb_auth_dump("qp * (m1 - m2)", m1);

    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&m1, &m1, &p));
    adb_auth_dump("h = qp * (m1 - m2) % p", m1);

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&m1, &m1, &q));
    adb_auth_dump("hq", m1);

    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&m1, &m2, &m1));
    adb_auth_dump("m = m2 + hq", m1);

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&m1, sig, SIGN_SIZE));
    ret = SIGN_SIZE;

cleanup:
    mbedtls_mpi_free(&p);
    mbedtls_mpi_free(&q);
    mbedtls_mpi_free(&dpq);
    mbedtls_mpi_free(&m);
    mbedtls_mpi_free(&m1);
    mbedtls_mpi_free(&m2);

    return ret;
}

#endif

