/** @file
  Defines additional cryptographic functions.

  Copyright 2014 - 2022 Infineon Technologies AG ( www.infineon.com )

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __CRYPT_FNC_IFX_H__
#define __CRYPT_FNC_IFX_H__

#include <Uefi/UefiBaseType.h>

/**
  Verify the given RSA PKCS#1 RSASSA-PSS signature.

  This function verifies the given RSA PKCS#1 RSASSA-PSS signature with a RSA 2048-bit public key.

  If MessageHash is NULL, then return FALSE.
  If HashSize is not equal to the size of SHA-256 digest, then return FALSE.
  If Signature is NULL, then return FALSE.
  If SigSize is not equal to the size of RSA2048 signature, then return FALSE.
  If Modulus is NULL, then return FALSE.
  If ModulusSize is not equal to the size of RSA2048 public key, then return FALSE.

  @param[in]       MessageHash      Pointer to octet message hash to be used for RSASSA-PSS verification.
  @param[in]       MessageHashSize  Size of the message hash in bytes.
  @param[in]       Signature        Pointer to buffer that carries the RSASSA-PSS signature.
  @param[in]       SignatureSize    Size of the Signature buffer in bytes.
  @param[in]       Modulus          Pointer to buffer that carries the RSA public key.
  @param[in]       ModulusSize      Size of the Modulus buffer in bytes.

  @retval  TRUE   Signature verification succeeded.
  @retval  FALSE  Signature verification failed, Buffer size is invalid or pointer is NULL.
*/
BOOLEAN
EFIAPI
RsaPssVerifyIfx (
  const UINT8   *MessageHash,
  const UINT32  MessageHashSize,
  const UINT8   *Signature,
  const UINT32  SignatureSize,
  const UINT8   *Modulus,
  const UINT32  ModulusSize
  );

/**
  Encrypt a byte array with a RSA 2048-bit public key

  This function encrypts the given data stream with RSA 2048-bit.

  If MessageHash is NULL, then return FALSE.
  If HashSize is not equal to the size of SHA-256 digest, then return FALSE.
  If Signature is NULL, then return FALSE.
  If SigSize is not equal to the size of RSA2048 signature, then return FALSE.
  If Modulus is NULL, then return FALSE.
  If ModulusSize is not equal to the size of RSA2048 public key, then return FALSE.

  @param[in]       InputDataSize       Size of the InputData buffer in bytes.
  @param[in]       InputData           Pointer to input data buffer.
  @param[in]       PublicModulusSize   Size of the PublicModulus buffer in bytes.
  @param[in]       PublicModulus       Pointer to modulus buffer.
  @param[in]       PublicExponentSize  Size of the PublicExponent buffer in bytes.
  @param[in]       PublicExponent      Pointer to public exponent buffer.
  @param[in,out]   EncryptedDataSize   Size of the EncryptedData buffer in bytes.
  @param[in,out]   EncryptedData       Pointer to encrypted data buffer.

  @retval  TRUE   Signature verification succeeded.
  @retval  FALSE  Signature verification failed, Buffer size is invalid or pointer is NULL.
*/
BOOLEAN
EFIAPI
RsaEncryptOaepIfx (
  UINT32       InputDataSize,
  const UINT8  *InputData,
  UINT32       PublicModulusSize,
  const UINT8  *PublicModulus,
  UINT32       PublicExponentSize,
  const UINT8  *PublicExponent,
  UINT32       *EncryptedDataSize,
  UINT8        *EncryptedData
  );

/**
  Calculate HMAC-SHA-1 on the given message.

  This function calculates a HMAC-SHA-1 on the input message.

  If InputMessage is NULL, then return FALSE.
  If InputMessageSize is zero, then return FALSE.
  If Key is NULL, then return FALSE.
  If KeySize is not equal to the of SHA1 digest, then return FALSE.
  If HmacResult is NULL, then return FALSE.
  If HmacResultSize is not equal to the size of SHA1 digest, then return FALSE.

  @param[in]        InputMessage         Pointer to input message buffer.
  @param[in]        InputMessageSize     Size of the InputMessage buffer in bytes.
  @param[in]        Key                  Pointer to message authentication key.
  @param[in]        KeySize              Size of the message authentication key buffer in bytes.
  @param[in,out]    HmacResult           Pointer to the HMAC result buffer in bytes.
  @param[in]        HmacResultSize       Size of to HMAC result buffer.

  @retval  TRUE   HMAC calculation succeeded.
  @retval  FALSE  HMAC calculation failed, Buffer size is invalid or pointer is NULL.
*/
BOOLEAN
EFIAPI
HmacSha1Ifx (
  const UINT8   *InputMessage,
  const UINT16  InputMessageSize,
  const UINT8   *Key,
  const UINT16  KeySize,
  const UINT8   *HmacResult,
  const UINT16  HmacResultSize
  );

#endif // __CRYPT_FNC_IFX_H__
