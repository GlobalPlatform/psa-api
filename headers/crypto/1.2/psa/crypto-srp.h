/* This file contains reference definitions for implementations of 
 * SRP-6 in addition to the PSA Certified Crypto API v1.2 PAKE Extension
 *
 * These definitions must be embedded in, or included by, psa/crypto.h
 */

#define PSA_KEY_TYPE_SRP_KEY_PAIR_BASE          ((psa_key_type_t) 0x7700)
#define PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE        ((psa_key_type_t) 0x4700)
#define PSA_KEY_TYPE_SRP_GROUP_MASK             ((psa_key_type_t) 0x00ff)

 /** SRP key pair. Both the client and server key.
 *
 * The size of a SRP key is the size associated with the Diffie-Hellman
 * group. See the documentation of each Diffie-Hellman group for details.
 * To construct a SRP key pair, the password hash must be imported.
 * The corresponding public key (password verifier) can be exported using
 * psa_export_public_key(). See also #PSA_KEY_TYPE_SRP_PUBLIC_KEY().
 *
 * \param group A value of type ::psa_dh_family_t that identifies the
 *              Diffie-Hellman group to be used.
 */
#define PSA_KEY_TYPE_SRP_KEY_PAIR(group) \
    ((psa_key_type_t) (PSA_KEY_TYPE_SRP_KEY_PAIR_BASE | (group)))

 /** SRP public key. The server key (password verifier).
 *
 * The size of an SRP public key is the same as the corresponding private
 * key. See #PSA_KEY_TYPE_SRP_KEY_PAIR() and the documentation of each
 * Diffie-Hellman group for details.
 * To construct a SRP public key, it must be imported. The key size
 * in attributes must not be zero.
 *
 * \param group A value of type ::psa_dh_family_t that identifies the
 *              Diffie-Hellman group to be used.
 */
#define PSA_KEY_TYPE_SRP_PUBLIC_KEY(group) \
    ((psa_key_type_t) (PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE | (group)))

 /** Whether a key type is a SRP key (pair or public-only). */
#define PSA_KEY_TYPE_IS_SRP(type)                                 \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) &                 \
      ~PSA_KEY_TYPE_SRP_GROUP_MASK) ==                            \
      PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE)
 /** Whether a key type is a SRP key pair. */
#define PSA_KEY_TYPE_IS_SRP_KEY_PAIR(type)                        \
    (((type) & ~PSA_KEY_TYPE_SRP_GROUP_MASK) ==                   \
     PSA_KEY_TYPE_SRP_KEY_PAIR_BASE)
 /** Whether a key type is a SRP public key. */
#define PSA_KEY_TYPE_IS_SRP_PUBLIC_KEY(type)                      \
    (((type) & ~PSA_KEY_TYPE_SRP_GROUP_MASK) ==                   \
     PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE)
 /** Extract the curve from a SRP key type. */
#define PSA_KEY_TYPE_SRP_GET_FAMILY(type)                         \
    ((psa_ecc_family_t) (PSA_KEY_TYPE_IS_SRP(type) ?              \
                         ((type) & PSA_KEY_TYPE_SRP_GROUP_MASK) : \
                         0))

/** The Secure Remote Passwort key exchange (SRP) algorithm.
 *
 * This is SRP-6 as defined by RFC 2945 and RFC 5054, instantiated with the
 * following parameters:
 *
 * - The group is defined over a finite field using a secure prime.
 * - A cryptographic hash function.
 *
 * To select these parameters and set up the cipher suite, call these functions:
 *
 * \code
 * psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
 * psa_pake_cs_set_algorithm(cipher_suite, PSA_ALG_SRP_6(hash));
 * psa_pake_cs_set_primitive(&cipher_suite,
 *                           PSA_PAKE_PRIMITIVE(type, family, bits));
 * \endcode
 *
 * After initializing a SRP operation, call:
 *
 * \code
 * psa_pake_setup(operation, password, cipher_suite);
 * psa_pake_set_role(operation, ...);
 * psa_pake_set_user(operation, ...);
 * \endcode
 * 
 * The password provided to the client side must be of type
 * #PSA_KEY_TYPE_SRP_KEY_PAIR.
 * The password provided to the server side must be of type
 * #PSA_KEY_TYPE_SRP_PUBLIC_KEY.
 *
 * The role set by \c psa_pake_set_role() must be either
 * \c PSA_PAKE_ROLE_CLIENT or \c PSA_PAKE_ROLE_SERVER.
 * 
 * For the SRP client key exchange call the following functions in any order:
 * \code
 * // get salt
 * psa_pake_input(operation, #PSA_PAKE_STEP_SALT, ...);
 * // get server key
 * psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // write client key
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * \endcode
 *
 * For the SRP server key exchange call the following functions in any order:
 * \code
 * // get salt
 * psa_pake_input(operation, #PSA_PAKE_STEP_SALT, ...);
 * // get client key
 * psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // write server key
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * \endcode
 *
 * For the client proof phase call the following functions in this order:
 * \code
 * // send M1
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // receive M2
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // Get secret
 * psa_pake_get_shared_key()
 * \endcode
 *
 * For the server proof phase call the following functions in this order:
 * \code
 * // receive M1
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // send M2
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // Get secret
 * psa_pake_get_shared_key()
 * \endcode
 *
 * The shared secret that is produced by SRP is pseudorandom. Although
 * it can be used directly as an encryption key, it is recommended to use
 * the shared secret as an input to a key derivation operation to produce
 * additional cryptographic keys.
 */
#define PSA_ALG_SRP_6_BASE                      ((psa_algorithm_t) 0x0a000300)
#define PSA_ALG_SRP_6(hash_alg) (PSA_ALG_SRP_6_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_IS_SRP_6(alg) (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_SRP_6_BASE)

/** The salt.
 *
 * The format for both input and output at this step is plain binary data.
 */
#define PSA_PAKE_STEP_SALT                      ((psa_pake_step_t)0x05)

/** Diffie-Hellman groups defined in RFC 3526.
 *
 * This family includes groups with the following key sizes (in bits):
 * 1536, 2048, 3072, 4096, 6144, 8192. A given implementation may support
 * all of these sizes or only a subset.
 */
#define PSA_DH_FAMILY_RFC3526                   ((psa_dh_family_t) 0x05)

#define PSA_ALG_SRP_PASSWORD_HASH_BASE          ((psa_algorithm_t) 0x08800300)
 /** The SRP password to password-hash KDF.
 * It takes the password p, the salt s, and the user id u.
 * It calculates the password hash h as
 * h = H(salt || H(u || ":" || p)
 * where H is the given hash algorithm.
 * 
 * This key derivation algorithm uses the following inputs, which must be
 * provided in the following order:
 * - #PSA_KEY_DERIVATION_INPUT_INFO is the user id.
 * - #PSA_KEY_DERIVATION_INPUT_PASSWORD is the password.
 * - #PSA_KEY_DERIVATION_INPUT_SALT is the salt.
 * The output has to be read as a key of type PSA_KEY_TYPE_SRP_KEY_PAIR.
 */
#define PSA_ALG_SRP_PASSWORD_HASH(hash_alg)                            \
    (PSA_ALG_SRP_PASSWORD_HASH_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

 /** Whether the specified algorithm is a key derivation algorithm constructed
 * using #PSA_ALG_SRP_PASSWORD_HASH(\p hash_alg).
 *
 * \param alg An algorithm identifier (value of type #psa_algorithm_t).
 *
 * \return 1 if \p alg is a key derivation algorithm constructed using #PSA_ALG_SRP_PASSWORD_HASH(),
 *         0 otherwise. This macro may return either 0 or 1 if \c alg is not a supported
 *         key derivation algorithm identifier.
 */
#define PSA_ALG_IS_SRP_PASSWORD_HASH(alg)                         \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_SRP_PASSWORD_HASH_BASE)

