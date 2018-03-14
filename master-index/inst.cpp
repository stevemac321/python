#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <cstring>
#include <algorithm>
#include <time.h>
#include <cctype>
#include <mysql/mysql.h>

template <typename T, size_t N> constexpr size_t _len(T (&)[N])
{
	return N;
}

struct book {
	std::string name = "";
	std::vector<std::pair<std::string, std::string>> verses;
	unsigned int count = 0;
};
char *trim_leading(char *line)
{
	char *p = line;

	while (isspace(*p) && *p != '\0')
		++p;

	line = p;
	return line;
}
char *trim_end(char *line)
{
	char *end = line;
	while (*end != '\0')
		++end;

	while (!isalnum(*end) && end > line) {
		*end = '\0';
		--end;
	}
	if (*end == '\r')
		*end = '\0';

	return line;
}

int main()
{
	const char *books[] = {"Genesis",
			       "Exodus",
			       "Leviticus",
			       "Numbers",
			       "Deuteronomy",
			       "Joshua",
			       "Judges",
			       "Ruth",
			       "1 Samuel",
			       "2 Samuel",
			       "1 Kings",
			       "2 Kings",
			       "1 Chronicles",
			       "2 Chronicles",
			       "Ezra",
			       "Nehemiah",
			       "Esther",
			       "Job",
			       "Psalm",
			       "Proverbs",
			       "Ecclesiastes",
			       "Song of Solomon",
			       "Isaiah",
			       "Jeremiah",
			       "Lamentations",
			       "Ezekiel",
			       "Daniel",
			       "Hosea",
			       "Joel",
			       "Amos",
			       "Obadiah",
			       "Jonah",
			       "Micah",
			       "Nahum",
			       "Habakkuk",
			       "Zephaniah",
			       "Haggai",
			       "Zechariah",
			       "Malachi",
			       "Matthew",
			       "Mark",
			       "Luke",
			       "John",
			       "Acts",
			       "Romans",
			       "1 Corinthians",
			       "2 Corinthians",
			       "Galatians",
			       "Ephesians",
			       "Philippians",
			       "Colossians",
			       "1 Thessalonians",
			       "2 Thessalonians",
			       "1 Timothy",
			       "2 Timothy",
			       "Titus",
			       "Philemon",
			       "Hebrews",
			       "James",
			       "1 Peter",
			       "2 Peter",
			       "1 John",
			       "2 John",
			       "3 John",
			       "Jude",
			       "Revelation",
			       "Tobit",
			       "Wisdom of Solomon",
			       "Ecclesiasticus",
			       "Baruch"};


	std::fstream fs("inst.txt");
	std::regex rch("^[A-Z].*");
	std::regex rchref("^(ch. [0-9]+).*([0-9]+.[0-9]+.[0-9]+)");
	std::regex rmchref("^(ch. [0-9]+; [0-9]+) .*([0-9]+.[0-9]+.[0-9]+)");
	std::regex rnumref("^([0-9]+:.*)([0-9]+.[0-9]+.[0-9]+)");
	std::smatch mch;
	std::string line = "";

	size_t booklen = _len(books);
	std::vector<book> v(booklen);
	for (size_t i = 0; i < booklen; i++)
		v[i].name = books[i];


	if (fs.is_open()) {
		size_t dex = 0;
		while (!fs.eof()) {
			std::getline(fs, line);
			char *trim = const_cast<char *>(line.c_str());
			trim = trim_leading(trim);
			trim = trim_end(trim);
			std::string trimmed(trim);
			if (std::regex_search(trimmed, mch, rch)) {
				for (size_t i = 0; i < booklen; i++) {
					if (v[i].name == trimmed)
						dex = i;
				}
			} else if (std::regex_search(trimmed, mch, rmchref)) {
				std::string vs = mch[1];
				std::string ref = mch[2];
                                char * trimvs = const_cast<char*>(vs.c_str());
                                char * trimref = const_cast<char*>(ref.c_str());
                                trimvs = trim_end(trimvs);
                                trimref = trim_end(trimref);
                                vs = trimvs;
                                std::string eref = "ICR ";
                                eref += trimref;
				auto p = std::make_pair(vs, eref);
				v[dex].verses.push_back(p);
			} else if (std::regex_search(trimmed, mch, rchref)) {
				std::string vs = mch[1];
				std::string ref = mch[2];
                                char * trimvs = const_cast<char*>(vs.c_str());
                                char * trimref = const_cast<char*>(ref.c_str());
                                trimvs = trim_end(trimvs);
                                trimref = trim_end(trimref);
                                vs = trimvs;
                                std::string eref = "ICR ";
                                eref += trimref;
				auto p = std::make_pair(vs, eref);
				v[dex].verses.push_back(p);
				v[dex].count++;
						v[dex].count++;
			} else if (std::regex_search(trimmed, mch, rnumref)) {
				std::string vs = mch[1];
				std::string ref = mch[2];
	                        char * trimvs = const_cast<char*>(vs.c_str());
                                char * trimref = const_cast<char*>(ref.c_str());
                                trimvs = trim_end(trimvs);
                                trimref = trim_end(trimref);
                                vs = trimvs;
                                std::string eref = "ICR ";
                                eref += trimref;
				auto p = std::make_pair(vs, eref);
				v[dex].verses.push_back(p);
				v[dex].count++;
			}
		}
		fs.close();
	}

	char insertbuf[512] = {'\0'};
	MYSQL *con = mysql_init(nullptr);
	assert(con);
	int sqlret = 0;

	assert(mysql_real_connect(con, "localhost", "root", "sqlpw",
				  "reformedrefs", 0, nullptr, 0));

	const char *fmt = "INSERT INTO reftable VALUES('%s','%s','%s')";

	for (size_t i = 0; i < booklen; i++) {
		if (v[i].count > 0) {
			char *bookname = const_cast<char *>(v[i].name.c_str());
			for (auto iv : v[i].verses) {
				sprintf(insertbuf, fmt, bookname,
					iv.first.c_str(), iv.second.c_str());
				puts(insertbuf);
				sqlret = mysql_query(con, insertbuf);
				if (sqlret != 0) {
					printf("failed: %d", sqlret);
					goto cleanup;
				}
			}
		}
	}
cleanup:
	mysql_close(con);
}

