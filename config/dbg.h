/*
 * Concept taken from: http://c.learncodethehardway.org/book/learn-c-the-hard-waych21.html
 */

#ifndef __dbg_h__
	#define __dbg_h__

	#include <errno.h> 

	#ifdef NDEBUG
		#define debug(M, ...)
		#define debug_clean(M, ...)
	#else 
		#define debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__) 
		#define debug_clean(M, ...) fprintf(stderr, "[DEBUG] " M "\n", ##__VA_ARGS__) 
	#endif

	#define clean_errno() (errno == 0 ? "" : strerror(errno))

	#define log_err(M, ...) fp_log = fopen(s_log_filename, "a"); fprintf(fp_log, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); fclose(fp_log)

	#define log_warn(M, ...) fp_log = fopen(s_log_filename, "a"); fprintf(fp_log, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); fclose(fp_log)

	#define log_info(M, ...) fp_log = fopen(s_log_filename, "a"); fprintf(fp_log, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__); fclose(fp_log)

	#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; } 

	#define check_exit(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; exit(EXIT_FAILURE); } 

	#define ASSERT(A, M, ...) if(!(A)) { fprintf(stderr, "error: %s: %s" M "\n", prog_name, clean_errno(), ##__VA_ARGS__); errno=0; exit(EXIT_FAILURE); } 

#endif
