CASTLE_S_S
# Fortress
[0] 5        p7f p8g b8h s6h g6i | P8d P3c
[1] 6        p8g p7f s8h b7g | P8e P3d
[2] 8        p8g p7f s7g p6g p5g g7h k6h g5h
[3] 5        b8h p8g p7f s7g g7h k6i
[4] 5        k5i g6i g6g s7h b8h p8g p7f p6f
[5] 8        k6i b7i g7h s7g p8g p7f
[6] 7        k6i s8h g7h g5h p8g b7g p7f p6f p5f
[7] 8        k6i g7h b6h g5h p8g s7g p7f p6f p5f
[8] 9 10     k7i g7h p8g p7f s7g p6f p5f g6g
[9] 11       k8h g7h p9f p8g p7f p6f s7g g6g | P9d
[10]         k7i g8h p9g p8g s7g g6g p7f p6f | P9e
[11]         l9i n8i k8h g7h s7g p8g p9f p7f
[12] 13      b7g | S7b
[13] 6       b7g s8h g7h | S8d
# without pawns
[14] 2       s7g g7h k6h g5h
[15] 8       k7i g7h g6g
[16] 7       k6i g7h b6h g5h s7g
[17] 11      l9i n8i k8h g7h s7g
[18] 6       k6i s8h g7h g5h p8g b7g
ATTACK_S_S
[0] 1 2      p5g p4g p3g p2f p1g r2h s3i
[1] 3        p5g p4g p3g p2f s4h r2h n2i
[2] 4        s3h p4g p3g p2f r2h
[3] 5 6 7    s4h p4g p3f p2f p1g r2h n2i
[4] 12 14 15 17 p3f p2e s2f p1f r2h n2i l1i
[5] 12       n2i r2h s3g p3f p2e p1e | P1d
[6] 9 11     l1i r2h s4g n3g p4f p3f p2f p1e | P1c
[7] 8        s4h p5f p4g n3g p3f p2f r2h l1g 
[8]          s4h p5f p4g n3g p3f p2f r1h l1g 
[9] 10       s4g p4f n3g n3f r2h l1g p1e
[10]         s4g p4f n3g n3f r1h l1g p1e
[11]         s5f p5g p4f p3f n3g r2h p2e p1f l1i
[12] 13      n2i s3g p3f p2e l1g r2h 
[13]         n2i s3g p3f p2e l1g r1h 
[14] 16      n2i l1i r2h p2e p3f s3e
[15] 16      n2i l1i r2h p2e p3e s2f
[16]         s3e p2d r2h
[17]         s1e p2d r2h
CASTLE_S_R
[0] 1 2      g6i g5h k5i b8h p7f s7i n8i l9i
[1] 3        p9f l9i n8i b8h k7h g6i g5h p8g p7f p6g p5f
[2] 4        l9i n8i s7i g6i g5h k6h b8h p8g p6g p7f
[3] 7 8      l9i n8i g6i b8h k7h s6h g5h p9f p8g p7f p6g p5f
[4] 5        l9i n8i s7i g6i g5h k7h b8h p8g p6g p7f p5g
[5] 6        l9h k8h n8i s7i g6i b7g p9g p8g p7f
[6]          k9i l9h s8h g7h g7i p8g b7g p7f p6g 
[7]          l9i n8i k8h g7h p9f p8g b7g g6g p7f p6f
[8] 9        l9i n8i b8h k7h g6i g5h s7g p7e p6g s5g p5f | P7c
[9]          l9i n8i k8h g7h b7g g6g s5g s7f p6f p5f p8e p7e
# without pawns
[10] 1      l9i n8i b8h k7h g6i g5h
[11] 3      l9i n8i g6i b8h k7h s6h g5h
[12] 2      l9i n8i s7i g6i g5h k6h b8h
ATTACK_S_R
[0] 1 2 6    r2h p2e
[1] 2 3 5 6 13 s4h r2h n2i l1i p5f p3f
[2] 13       l1i s4h r2h n3g p4f p3f
# Center-file Vanguard Pawn
[3] 4        l1i n2i r2h s6h p3g p4g s5g p5e | P5c P4d
[4]          l1i n2i r2h s6h p3g p4g s5f p5e | P5c
# King's Head Vanguard Pawn
[5]          l1i b8h s7g s5g p7e | P7c
# rushing silver
[6] 7        p2e p1f r2h s3h
[2] 8 10     p2e p3f p1f s2f r2h
[8] 9        p3e p2e p1f s2f r2h
[9]          s3e p2e p1f r2h
[10]         p2e s1e p1f r2h
# edge attack 
[11] 12      l1g r2h
[12]         l1g r1h p1d     
# slow
[13] 11      s4g n3g r2h l1i p4f p3f
CASTLE_R_S
# going into Mino or Bear-in-the-Hole
[0] 1        g6i g4i s3i n2i l1i k4h p4g p3g p2g p1g
[1] 2 4      g6i g4i s3i n2i l1i k2h p4g p3g p2g p1g
# Bear-in-the-Hole
[2] 3        g4i s3i n2i k1i g5h l1h p4g p3g p2g p1g
[3] 12       g3i n2i k1i g3h s2h l1h p4g p3g p2g p1g
# Incomplete Mino
[4] 5 8      g4i n2i l1i s3h k2h p3g p2g p1f
# Complete Mino
[5] 6 9      g4i n2i l1i g5h s3h k2h p4g p3g p2g p1f
# High Mino
[6] 7 10     g4i n2i l1i s3h k2h g4g p2g p4f p3f p1f
# Silver Crown
[7] 11       n2i l1i g3h k2h g4g s2g p4f p3f p2f p1f
# Incomplete Mino without pawns
[8] 5        g4i n2i l1i s3h k2h p3g p2g p1f
# Complete Mino without pawns
[9] 6        g4i n2i l1i g5h s3h k2h p4g p3g p2g p1f
# High Mino without pawns
[10] 7       g4i n2i l1i s3h k2h g4g 
# Silver Crown without pawns
[11]         n2i l1i g3h k2h g4g s2g
# Bear-in-the-Hole without pawns
[12]         g3i n2i k1i g3h s2h l1h
CASTLE_R_R
[0] 1        l1i s3i g4i g6i k3h p2g p3g p4g
[1] 2        l1i g4i s2h k3h g5h p2g p3g p4g
[2]          s2h k3h g4h g5h p2g p3g p4g
# without pawns
[3] 0        l1i s3i g4i g6i k3h
[4] 1        l1i g4i s2h k3h g5h
[5] 2        s2h k3h g4h g5h
CASTLE_S_U
[0]          p1g p2e p3g p4g p5g p6g p7f p8g p9g n8i s7i g6i k5i g4i s3i n2i
[1]          p8g p7f p6g g6i g5h p5f
ATTACK_S_U
[0] 1 2      p2f r2h
[1] 3        p2f s4h
[2] 6        p2f s3h
[3] 4 5      p2f s4h p3f
[4]          p2f p4f p3f s4g n3g
[5]          p2e p3f s3g p1e
[6] 7        p2e s2g p3g
[7]          p2e s2f p1f p3f
CASTLE_R_U
[0] 4        r5h k5i g4i s3i
[1] 4        r6h k5i g4i s3i
[2] 4        r7h k5i g4i s3i
[3] 4        r8h k5i g4i s3i
[4]          k3h g4i s3i
ATTACK_R_U
[0] 1 2 3 4  p7f p6f b7g
[1]          r8h p7f p6f b7g s6g
[2]          r7h p7f p6f b7g
[3]          r6h p7f p6f b7g s6g
[4]          r5h p7f p6f b7g s6g
CASTLE_U_U
[0] 1        p3g p4g p6g p7f n8i s7i g6i k5i g4i s3i n2i
[1] 2 3      p3g p4g p6f p7f b8h s7i g6i k5i g4i s3i
[2] 4        p6f p7f s7h b8h
[3] 4        p6f p7f s6h b8h
[4]          p6f p7f s6h b8h p1e
ATTACK_U_U
[0]          p7f
ATTACK_R_*
# going into Ranging Rook
[0] 1 5 8 10 b7g p7f p6f k5i g4i s3i p2g
# 2nd file Ranging Rook
[1] 2        r8h b7g p7f p6f k5i g4i s3i
[2] 3 4      r8h n8i g6i b7g s6g p7f p6f
[3]          r8h n8i g7h b7g s6g p7f p6f
[4]          r8h n8i g5h b7g s6g p7f p6f
# 3rd file Ranging Rook
[5] 6 7      r7h b7g p7f p6f k5i g4i s3i
[6]          r7h s6g g5h p8g b7g p7f p6f
[7]          r7h s6g p8g b7g p7f p6f p5f
# 4th file Ranging Rook
[8] 9        r6h b7g s6g p7f p6f k5i g4i s3i
[9]          r6h n8i g6i p8g b7g s6g p7f p6f
# 5th file Ranging Rook
[10] 11 13   r5h p8g b7g s6g k5i g4i s3i 
[11] 12      r5h n8i g7h p8g b7g s6g p7f p6f
[12]         r5i n8i p8g b7g s6g s4g p7f p6f
[13]         r5h n8i p8g b7g s6g g4g p7f p6f
CASTLE_U_*
[0] 1        p3g p4g p6g p7f n8i s7i g6i k5i g4i s3i n2i
[1] 2 3      p3g p4g p6f p7f b8h s7i g6i k5i g4i s3i
[2] 4        p6f p7f s7h b8h
[3] 4        p6f p7f s6h b8h
[4]          p6f p7f s6h b8h p1e
