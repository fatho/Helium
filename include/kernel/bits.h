/**
 * @file bits.h
 *
 * @author fabian
 * @date   19.06.2014
 *
 * @brief Contains macros for bit manipulations
 */
#ifndef BITS_H_
#define BITS_H_

#define HAS_FLAG(val,flag) ((val)&(flag))
#define SET_FLAG(val,flag) ((val)|(flag))
#define UNSET_FLAG(val,flag) ((val)& ~(flag))
#define TOGGLE_FLAG(val,flag) ((val)&(flag))


#define ALIGN_FLOOR(addr,power) ((val)&((1<<(power))-1))
#define ALIGN_CEIL(addr,power) ((val+((1<<(power))-1))&((1<<(power))-1))


#endif /* BITS_H_ */
