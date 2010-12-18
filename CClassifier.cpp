#include "CClassifier.h"

const CClassifier::ECharacterGroup CClassifier::ms_eClassifier [ 256 ] = {
//      00 NUL  01 SOH  02 STX  03 ETX  04 EOT  05 ENQ  06 ACK  07 BEL  08 BS   09 HT
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   SPACE,
//      0A NL   0B VT   0C NP   0D CR   0E SO   0F SI   10 DLE  11 DC1  12 DC2  13 DC3
        LF,     UNKN,   UNKN,   CR,     UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
//      14 DC4  15 NAK  16 SYN  17 ETB  18 CAN  19 EM   1A SUB  1B ESC  1C FS   1D GS
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
//      1E RS   1F US   20 SP   21  !   22  "   23  #   24  $   25  %   26  &   27  '
        UNKN,   UNKN,   SPACE,  UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
//      28  (   29  )   2A  *   2B  +   2C  ,   2D  -   2E  .   2F  /   30  0   31  1
        SEP,    SEP,    STAR,   PLUS,   SEP,    MINUS,  DOT,    SLASH,  DIGIT,  DIGIT,
//      32  2   33  3   34  4   35  5   36  6   37  7   38  8   39  9   3A  :   3B  ;
        DIGIT,  DIGIT,  DIGIT,  DIGIT,  DIGIT,  DIGIT,  DIGIT,  DIGIT,  SEP,    SEP,
//      3C  <   3D  =   3E  >   3F  ?   40  @   41  A   42  B   43  C   44  D   45  E
        LT,     EQUAL,  GT,     UNKN,   UNKN,   ALPHA,  ALPHA,  ALPHA,  ALPHA,  eE,
//      46  F   47  G   48  H   49  I   4A  J   4B  K   4C  L   4D  M   4E  N   4F  O
        ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
//      50  P   51  Q   52  R   53  S   54  T   55  U   56  V   57  W   58  X   59  Y
        ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
//      5A  Z   5B  [   5C  \   5D  ]   5E  ^   5F  _   60  `   61  a   62  b   63  c
        ALPHA,  SEP,    UNKN,   SEP,    UNKN,   UNDER,  UNKN,   ALPHA,  ALPHA,  ALPHA,
//      64  d   65  e   66  f   67  g   68  h   69  i   6A  j   6B  k   6C  l   6D  m
        ALPHA,  eE,     ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
//      6E  n   6F  o   70  p   71  q   72  r   73  s   74  t   75  u   76  v   77  w
        ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
//      78  x   79  y   7A  z   7B  {   7C      7D  }   7E  ~   7F DEL
        ALPHA,  ALPHA,  ALPHA,  UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        SPACE,  UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN,
        UNKN,   UNKN,   UNKN,   UNKN,   UNKN,   UNKN
    };
