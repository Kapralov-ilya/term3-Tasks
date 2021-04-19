#include <iostream>
#include <fcntl.h>
#include <unistd.h>

enum Marker{
	identifier,
	keyword,
	constant,
	string,
	operation,
	punctuator,
	defect,
	note
};

struct LexItem{
	const char *word;
	unsigned int line;
	marker type;
	LexItem *next;
};

class Scanner{
	enum State{
		home,
		ident,
		keywrd,
		count,
		quote,
		equal,
		comment,
	};
	State flag;
	bool delay;
	bool separate;
	unsigned int current_line;
	unsigned int unit;
	unsigned int buf_used;
	char *buffer;
	LexItem *token;
private:
	void Step(char symbol);
	void HandleHome(char symbol);
	void HandleIdent(char symbol);
	void HandleKeyword(char symbol);
	void HandleCount(char symbol);
	void HandleQuote(char symbol);
	void HandleEqual(char symbol);
	void HandleComment(char symbol);
	void AddLexeme(marker note);
	bool IsOperation(char symbol);
	bool CheckKeyword();
	void CheckDelimiter(char symbol);
	void Append(char symbol);
	void AllocateBuffer();
	void DisposeBuffer();
	char *GetString() const;
public:
	Scanner();
	~Scanner();
	void Feed(char symbol);
	LexItem *GetTokens();
};

Scanner::Scanner(){
	flag=home;
	delay=false;
	separate=false;
	current_line=1;
	unit=0;
	buf_used=0;
	buffer=0;
	token=0;
}

Scanner::~Scanner(){
	DisposeBuffer();
}

void Scanner::Feed(char symbol){
	Step(symbol);
	if (delay){
	delay=false;
		Step(symbol);
	}
	if (symbol=='\n'){
		current_line++;
	}
}

LexItem *Scanner::GetTokens(){
	LexItem *tempary;
	if (flag!=home){
		tempary=token;
		while (tempary->next){
			tempary=tempary->next;
		}
	}
	return token;
}

void Scanner::Step(char symbol){
	CheckDelimiter(symbol);
	switch (flag){
		case home:
			HandleHome(symbol);
			break;
        		case ident:
			HandleIdent(symbol);
			break;
		case keywrd:
			HandleKeyword(symbol);
			break;
		case count:
			HandleCount(symbol);
			break;
		case quote:
			HandleQuote(symbol);
			break;
		case equal:
			HandleEqual(symbol);
			break;
		case comment:
			HandleComment(symbol);
			break;
	}
}

void Scanner::HandleHome(char symbol){
	if (symbol==' '||symbol=='\t'||symbol=='\n') return;
	if (symbol=='#'){
		flag=comment;
		return;
	}
	if (symbol=='"'){
		flag=quote;
		return;
	}
	Append(symbol);
	if (IsOperation(symbol)){
		AddLexeme(operation);
		return;
	}
	if (symbol=='{' || symbol=='}'){
		AddLexeme(punctuator);
		return;
	}
	if (symbol=='$' || symbol=='?'){
		flag=ident;
		return;
	}
	if (symbol>='a' && symbol<='z'){
		flag=keywrd;
		return;
	}
	if (symbol>='0' && symbol<='9'){
		flag=count;
		return;
	}
	if (symbol=='='){
		flag=equal;
		return;
	}
	AddLexeme(defect);
}

void Scanner::HandleIdent(char symbol){
	if ((symbol>='a' && symbol<='z') || (symbol>='A' && symbol<='Z')
			|| (symbol>='0' && symbol<='9') || symbol=='_'){
		Append(symbol);
	}else{
		if (true){
			AddLexeme(identifier);
			delay=true;
			separate=true;
			flag=home;
		}else{
			AddLexeme(defect);
		}
	}
}

void Scanner::HandleKeyword(char symbol){
	if (symbol>='a' && symbol<='z'){
		Append(symbol);
	}else{
		if (CheckKeyword()){
			AddLexeme(keyword);
			delay=true;
			separate=true;
			flag=home;
		}else{
			AddLexeme(defect);
		}
	}
}

void Scanner::HandleCount(char symbol){
	if (symbol>='0' && symbol<='9'){
		Append(symbol);
	}else{
		AddLexeme(constant);
		delay=true;
		separate=true;
		flag=home;
	}
}

void Scanner::HandleQuote(char symbol){
	if (symbol!='"'){
		Append(symbol);
	}else{
		AddLexeme(string);
		separate=true;
		flag=home;
	}
}

void Scanner::HandleEqual(char symbol){
	if (symbol=='=' || symbol=='!' || symbol=='<' || symbol=='>'){
		Append(symbol);
		AddLexeme(operation);
		flag=home;
	}else{
		AddLexeme(operation);
		delay=true;
		flag=home;
	}
}

void Scanner::HandleComment(char symbol){
	if (symbol=='\n'){
		flag=home;
	}
}

void Scanner::AddLexeme(Marker note){
	LexItem *tempary;
	if (buf_used==0) return;
	if (token){
		tempary=token;
		while (tempary->next){
			tempary=tempary->next;
		}
		tempary->next=new LexItem;
		tempary=tempary->next;
	}else{
		token=new LexItem;
		tempary=token;
	}
	tempary->next=0;
	tempary->word=GetString();
	tempary->line=current_line;
	tempary->type=note;
	DisposeBuffer();
}

bool Scanner::IsOperation(char symbol){
	const char str[]="+-*/&|!<>()[];,";
	for(int i=0; str[i];i++){
		if (str[i]==symbol) return true;
	}
	return false;
}

bool Scanner::CheckKeyword(){
	const char *const keywords[]={"if","while",0};
	char *lexem=GetString();
	for(int i=0; keywords[i];i++){
		if (!strcmp(lexem, keywords[i])){
			delete []lexem;
			return true;
		}
	}
	delete []lexem;
	return false;
}

void Scanner::CheckDelimiter(char symbol){
	if (separate){
		separate=false;
		if (symbol==' ' || symbol=='\t' || symbol=='\n') return;
		if (symbol=='{' || symbol=='}' || symbol=='=') return;
		if (IsOperation(symbol)) return;
		Append(symbol);
		AddLexeme(defect);
	}
}

void Scanner::Append(char symbol){
	if (buf_used==unit*16){
		AllocateBuffer();
	}
	buffer[buf_used]=symbol;
	buf_used++;
}

void Scanner::AllocateBuffer(){
	unsigned int i, old_size;
	old_size=unit*16;
	unit++;
	char *tempary=new char[unit*16];
	for(i=0; i<old_size; i++){
		tempary[i]=buffer[i];
	}
	if (buffer){
		delete []buffer;
	}
	buffer=tempary;
}

void Scanner::DisposeBuffer(){
	unit=0;
	buf_used=0;
	if (buffer){
		delete []buffer;
		buffer=0;
	}
}

char *Scanner::GetString() const {
	char *str=new char[buf_used+1];
	for(unsigned int i=0;i<buf_used;i++){
		str[i]=buffer[i];
	}
	str[buf_used]='\0';
	return str;
}

int main(int argc, char **argv){
	int file_descriptor,character;
	Scanner Flow;
	file_descriptor=open(argv[1],O_RDONLY);
	dup2(file_descriptor,0);
	close(file_descriptor);
	character=getchar();
	while(character!=EOF){
		Flow.Feed(character);
		character=getchar();
	}
	Flow.GetTokens();
}
