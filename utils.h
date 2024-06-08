#ifndef __UTILS__
#define __UTILS__

typedef enum{

    FALSE,
    TRUE
}bool_t;

/* This is for checking that the bit on the specific position of the 
integer 'n' is set or not (here we do bitwise AND)*/
#define IS_BIT_SET(n, pos)  ((n & (1 << (pos))) != 0)

/* This is for changing or toggling the specific bit in the integer 'n'
it does this by XORing the bit with '1'*/
#define TOGGLE_BIT(n, pos)  (n = n ^ (1 << (pos)))

/* This is complementing the number by XORing by '1' */
#define COMPLEMENT(num)     (num = num ^ 0xFFFFFFFF)


#define UNSET_BIT(n, pos)   (n = n & ((1 << pos) ^ 0xFFFFFFFF))
#define SET_BIT(n, pos)     (n = n | 1 << pos)


#endif /*  __UTILS__ */