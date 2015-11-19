/**
 * similar.c
 *
 * 文字列の類似度を取得する
 * この版では1文字までのtypoを許容する
 *
 * str1はDB内などの「比較される」文字列
 * str2は「比較したい文字列」を想定
 * str1, str2ともに「正規化」済みの文字列を想定する。
 * 「正規化」については normalize.c を参照。
 *
 * return値は0～1.0までの値域とする。
 * 0は全く一致しない。
 * 1.0は完全一致
 *
 * 類似度として何を考慮するか。
 * - 比較対象の文字数と一致した文字数
 * 
 * メモ
 * - typoの種別：余剰、欠損、置換により減点係数を変えるべきか。
 * - 最初と最後の文字が合っていれば中間文字順序が入れ替わっていても
 *   問題なく判断できてしまう理論を導入すべきか。
 * - qwerty配列の考慮は日本語では困難であるため、当面実装しない。
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "similar.h"

#define TRUE 1
#define FALSE 0

float
similar_rate(wchar_t* w_n_str1, wchar_t* w_n_str2) {
    float rate;
    float matches = 0.0;
    int offset1 = 0; 
    int offset2 = 0; 
    int length1;
    int length2;
    int loop_cnt;
    int loop_limit;
    int max_length;
    int del1_flag = FALSE;
    int del2_flag = FALSE;

    length1 = wcslen(w_n_str1);
    length2 = wcslen(w_n_str2);
    max_length = (length1 > length2) ? length1 : length2 ;
    loop_limit = (length1 > length2) ? length2 : length1 ;
    
    /* byte長が短いほうを軸にループする */
    for (loop_cnt=0; loop_cnt < loop_limit; loop_cnt++) {
        /* 欠損フラグは一旦FALSEにする */
        del1_flag = FALSE;
        del2_flag = FALSE;
        if ( *(w_n_str1 + offset1) == *(w_n_str2 + offset2) ) {
            matches += 1.0;
            offset1++;
            offset2++;
        } else {
            /* 異なっている場合、相互に次のwchar_tと比較 */
            if ( *(w_n_str1+offset1) == *(w_n_str2+offset2+1) ) {
                /* str1が欠損の可能性 */
                matches += 0.25;
                del1_flag = TRUE;
                offset2++;
            }
            if ( *(w_n_str1+offset1 + 1 ) == *(w_n_str2+offset2) ) {
                /* str2が欠損の可能性 */
                matches += 0.25;
                del2_flag = TRUE;
                offset1++;
            }
            if ((del1_flag == FALSE) && (del2_flag == FALSE)) {
                /* 欠損ではないだろう */
                offset1++;
                offset2++;
            }
        }
    }

    rate =  matches / (float) max_length;

    return rate;
}

#ifdef SIMILAR_UNIT_TEST
int
main(int argc, char** argv) {
    static float rate;

    /* ワイドキャラ格納 */
    wchar_t* w_str1; /* 正規化前 */
    wchar_t* w_str2; /* 正規化前 */
    wchar_t* w_n_str1; /* 正規化後 */
    wchar_t* w_n_str2; /* 正規化後 */
    int w_str1_len, w_str2_len, w_n_str1_len, w_n_str2_len;

    if (argc != 3) exit(0);

    setlocale( LC_CTYPE, "ja_JP.UTF-8" );

    printf("str1=%s\n", argv[1]);
    printf("str2=%s\n", argv[2]);

    w_str1 = (wchar_t*) malloc(sizeof(wchar_t) * strlen(argv[1] + 1));
    w_str2 = (wchar_t*) malloc(sizeof(wchar_t) * strlen(argv[2] + 1));
    w_n_str1 = (wchar_t*) malloc(sizeof(wchar_t) * strlen(argv[1] + 1));
    w_n_str2 = (wchar_t*) malloc(sizeof(wchar_t) * strlen(argv[2] + 1));
    /* 念のためクリア */
    memset(w_str1, '\0', sizeof(wchar_t) * strlen(argv[1]) + 1);
    memset(w_str2, '\0', sizeof(wchar_t) * strlen(argv[2]) + 1);
    memset(w_n_str1, '\0', sizeof(wchar_t) * strlen(argv[1]) + 1);
    memset(w_n_str2, '\0', sizeof(wchar_t) * strlen(argv[2]) + 1);

    /* マルチバイトからワイドキャラへ変換 */
    mbstowcs( w_str1, argv[1], strlen(argv[1]));
    w_str1_len = wcslen(w_str1);
    mbstowcs( w_str2, argv[2], strlen(argv[2]));
    w_str2_len = wcslen(w_str2);
    /* 日本語正規化処理 */
    ntext_normalize(w_str1, w_n_str1, w_str1_len);
    w_n_str1_len = wcslen(w_n_str1);
    ntext_normalize(w_str2, w_n_str2, w_str2_len);
    w_n_str2_len = wcslen(w_n_str2);
    printf("w_n_str1=%ls, w_n_str1_len=%d\n", w_n_str1, w_n_str1_len);
    printf("w_n_str2=%ls, w_n_str2_len=%d\n", w_n_str2, w_n_str2_len);
    rate = similar_rate(w_n_str1, w_n_str2);
    printf("rate = %f\n", rate);
}
#endif // SIMILAR_UNIT_TEST
