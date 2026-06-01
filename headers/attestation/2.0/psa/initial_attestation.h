// SPDX-FileCopyrightText: Copyright 2018-2020, 2022-2026 Arm Limited and/or its affiliates
// SPDX-License-Identifier: Apache-2.0

/* This file is a reference template for implementation of the
 * PSA Certified Attestation API v2.0
 */

#ifndef PSA_INITIAL_ATTESTATION_H
#define PSA_INITIAL_ATTESTATION_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief The major version of this implementation of the Attestation API.
 */
#define PSA_INITIAL_ATTEST_API_VERSION_MAJOR 2

/**
 * @brief The minor version of this implementation of the Attestation API.
 */
#define PSA_INITIAL_ATTEST_API_VERSION_MINOR 0

/**
 * @brief The maximum possible size of a token.
 */
#define PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE /* implementation-specific value */

/**
 * @brief A challenge size of 32 bytes (256 bits).
 */
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 (32u)

/**
 * @brief A challenge size of 48 bytes (384 bits).
 */
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 (48u)

/**
 * @brief A challenge size of 64 bytes (512 bits).
 */
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64 (64u)

/**
 * @brief Retrieve the Initial Attestation Token.
 *
 * @param auth_challenge Buffer with a challenge object.
 * @param challenge_size Size of the buffer auth_challenge in bytes.
 * @param token_buf      Output buffer where the attestation token is to be
 *                       written.
 * @param token_buf_size Size of token_buf.
 * @param token_size     Output variable for the actual token size.
 */
psa_status_t psa_initial_attest_get_token(const uint8_t *auth_challenge,
                                          size_t challenge_size,
                                          uint8_t *token_buf,
                                          size_t token_buf_size,
                                          size_t *token_size);

/**
 * @brief Calculate the size of an Initial Attestation Token.
 *
 * @param challenge_size Size of a challenge object in bytes.
 * @param token_size     Output variable for the token size.
 */
psa_status_t psa_initial_attest_get_token_size(size_t challenge_size,
                                               size_t *token_size);

#ifdef __cplusplus
}
#endif

#endif // PSA_INITIAL_ATTESTATION_H
