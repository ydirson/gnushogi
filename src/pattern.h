/* #define TEST_PATTERN  */
/* #define DEBUG_PATTERN */


#define MAX_PATTERN 20 /* maximum number of pieces in pattern */
#define MAX_NEXT 6     /* maximum number of links */
#define MAX_NAME 20    /* maximum length of opening name */


typedef
  struct PatternField_rec {
    small_short side, piece, square;
  } PatternField;

typedef
  struct Pattern_rec {
    short n;
    PatternField field[MAX_PATTERN];
  } PatternFields;

typedef
  struct PatternSequence_rec { 
    small_short n;
    small_short next[MAX_NEXT];
    small_short distance[2];
    small_short reachedGameCnt[2];
    small_short visited;
    PatternFields patternfields;
    struct PatternSequence_rec *next_pattern;
  } PatternSequence;
               

typedef
  struct OpeningPattern_rec {
    char name[MAX_NAME];
    short n;
    PatternSequence *sequence;
    struct OpeningPattern_rec *next;
  } OpeningPattern;


extern OpeningPattern *Patterns;
    

extern
  short
  string_to_board_color
    (char *s);

extern
  short
  string_to_patternfields
    (char *s, PatternFields *pattern);

extern 
  short
  piece_to_pattern_distance 
    (short side, short piece, short pside, PatternFields *pattern);

extern
  short
  pattern_distance (short pside, PatternFields *pattern);

extern
  short
  board_to_pattern_distance 
    (short pside, OpeningPattern *opattern, short pmplty, short GameCnt);
 
extern
  OpeningPattern *locate_opening_pattern(short pside, char *s, short GameCnt);

extern
  void
  DisplayPattern (PatternFields *pattern);

extern
  void
  GetOpeningPatterns ();

extern
  void
  ShowOpeningPatterns ();



