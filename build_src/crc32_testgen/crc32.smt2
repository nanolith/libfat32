; A reference implementation of a bit-by-bit implementation of CRC-32.
; We can use this to derive test vectors to verify the table-driven C
; implementation.

(set-logic AUFBV)

; Constants
(define-fun poly () (_ BitVec 32) #xEDB88320)
(define-fun bv-one-32 () (_ BitVec 32) #x00000001)
(define-fun initial-val () (_ BitVec 32) #xFFFFFFFF)
(define-fun final-xor () (_ BitVec 32) #xFFFFFFFF)

; Handle a single bit-wise step of CRC-32.
(define-fun crc-bit-step ((crc-in (_ BitVec 32))) (_ BitVec 32)
  (let ((shifted (bvlshr crc-in bv-one-32)))
    (let ((bitval ((_ extract 0 0) crc-in)))
        (ite (= bitval #b1)
            (bvxor shifted poly)
            shifted))))

; Expand this bit-wise step into a byte-wise function.
(define-fun crc-byte-step
        ((crc-in (_ BitVec 32)) (byte-in-8b (_ BitVec 8))) (_ BitVec 32)
  (let ((s0 (bvxor crc-in ((_ zero_extend 24) byte-in-8b))))
    (let ((s1 (crc-bit-step s0)))
      (let ((s2 (crc-bit-step s1)))
        (let ((s3 (crc-bit-step s2)))
          (let ((s4 (crc-bit-step s3)))
            (let ((s5 (crc-bit-step s4)))
              (let ((s6 (crc-bit-step s5)))
                (let ((s7 (crc-bit-step s6)))
                  (let ((s8 (crc-bit-step s7)))
                    s8))))))))))

(define-fun-rec crc-recursive-loop
  (
    (data (Array (_ BitVec 32) (_ BitVec 8))) ; input array
    (len (_ BitVec 32))                       ; length
    (idx (_ BitVec 32))                       ; current index
    (crc-in (_ BitVec 32))                    ; current crc state
  )
  (_ BitVec 32)
  
  (ite (bvuge idx len)
        ; base case
        crc-in

        ; recursive case
        (crc-recursive-loop
          data
          len
          (bvadd idx #x00000001)
          (crc-byte-step crc-in (select data idx)))))

; Public function for C to call.
(define-fun crc-of-array
        ((data (Array (_ BitVec 32) (_ BitVec 8))) (len (_ BitVec 32)))
        (_ BitVec 32)
  (let ((intermediate-crc (crc-recursive-loop data len #x00000000 initial-val)))
    (bvxor intermediate-crc final-xor)))
