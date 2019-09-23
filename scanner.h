#ifndef scanner_h
#define scanner_h

#include <ostream>
#include <vector>

#define unq(x, ...) std::make_unique<x>(__VA_ARGS__)

typedef enum {
	TOKEN_LEFT_PAREN,  // (
	TOKEN_RIGHT_PAREN, // )

	TOKEN_AND,        // &
	TOKEN_OR,         // |
	TOKEN_COMPLEMENT, // !

	TOKEN_IDENTIFIER, // consecutive letters

	TOKEN_ERROR,
	TOKEN_EOF
} TokenType;

class Scanner;

typedef struct Token {
	TokenType   type;
	const char *start;
	const char *source;
	int         length;
	int         line;
	const char *fileName;

	static Token         from(TokenType t, Scanner *s);
	static Token         errorToken(const char *message, Scanner *s);
	friend std::ostream &operator<<(std::ostream &os, const Token &t);
	friend std::ostream &operator<<(std::ostream &            os,
	                                const std::vector<Token> &tv);
	void highlight(bool showFileName = false, const char *prefix = NULL) const;
	static const char *TokenNames[];
	static const char *FormalNames[];

	static Token PlaceholderToken;
} Token;

class Scanner {
  private:
	const char *       source;
	const char *       tokenStart;
	const char *       current;
	const char *       fileName;
	int                line;
	int                scanErrors;
	std::vector<Token> tokenList;

	// Returns 1 if `c` is an English letter or underscore.
	static bool isAlpha(char c);

	// Returns 1 if `c` is a digit.
	static bool isDigit(char c);

	// Returns 1 if `c` is an English letter, underscore, or digit.
	static bool isAlphaNumeric(char c);

	bool  isAtEnd();
	char  advance();
	char  peek();
	char  peekNext();
	bool  match(char expected);
	Token identifier();
	int   skipEmptyLine();

  public:
	Scanner(const char *source, const char *file);
	Scanner(const char *file);
	Token                     scanNextToken();
	const std::vector<Token> &scanAllTokens();
	bool                      hasScanErrors();

	friend Token;
};

#endif
