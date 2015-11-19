/**
 * normalize.c
 * 日本語テキストの正規化を行う
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>

typedef struct st_offset {
    int src;
    int dest;
} offset_t;

typedef struct st_kana_map {
    wchar_t src;
    wchar_t dest;
} kana_map_t;


/**
 * ひらがな->カタカナ変換表 */
kana_map_t HIRAGANA_MAP[] = {
  {L'ぁ', L'ア'},
  {L'あ', L'ア'},
  {L'ぃ', L'イ'},
  {L'い', L'イ'},
  {L'ぅ', L'ウ'},
  {L'う', L'ウ'},
  {L'ぇ', L'エ'},
  {L'え', L'エ'},
  {L'ぉ', L'オ'},
  {L'お', L'オ'},
  {L'か', L'カ'},
  {L'き', L'キ'},
  {L'く', L'ク'},
  {L'け', L'ケ'},
  {L'こ', L'コ'},
  {L'が', L'ガ'},
  {L'ぎ', L'ギ'},
  {L'ぐ', L'グ'},
  {L'げ', L'ゲ'},
  {L'ご', L'ゴ'},
  {L'さ', L'サ'},
  {L'し', L'シ'},
  {L'す', L'ス'},
  {L'せ', L'セ'},
  {L'そ', L'ソ'},
  {L'ざ', L'ザ'},
  {L'じ', L'ジ'},
  {L'ず', L'ズ'},
  {L'ぜ', L'ゼ'},
  {L'ぞ', L'ゾ'},
  {L'た', L'タ'},
  {L'ち', L'チ'},
  {L'っ', L'ツ'},
  {L'つ', L'ツ'},
  {L'て', L'テ'},
  {L'と', L'ト'},
  {L'だ', L'ダ'},
  {L'ぢ', L'ヂ'},
  {L'づ', L'ヅ'},
  {L'で', L'デ'},
  {L'ど', L'ド'},
  {L'な', L'ナ'},
  {L'に', L'ニ'},
  {L'ぬ', L'ヌ'},
  {L'ね', L'ネ'},
  {L'の', L'ノ'},
  {L'は', L'ハ'},
  {L'ひ', L'ヒ'},
  {L'ふ', L'フ'},
  {L'へ', L'ヘ'},
  {L'ほ', L'ホ'},
  {L'ば', L'バ'},
  {L'び', L'ビ'},
  {L'ぶ', L'ブ'},
  {L'べ', L'ベ'},
  {L'ぼ', L'ボ'},
  {L'ぱ', L'パ'},
  {L'ぴ', L'ピ'},
  {L'ぷ', L'プ'},
  {L'ぺ', L'ペ'},
  {L'ぽ', L'ポ'},
  {L'ま', L'マ'},
  {L'み', L'ミ'},
  {L'む', L'ム'},
  {L'め', L'メ'},
  {L'も', L'モ'},
  {L'ゃ', L'ヤ'},
  {L'や', L'ヤ'},
  {L'ゅ', L'ユ'},
  {L'ゆ', L'ユ'},
  {L'ょ', L'ヨ'},
  {L'よ', L'ヨ'},
  {L'ら', L'ラ'},
  {L'り', L'リ'},
  {L'る', L'ル'},
  {L'れ', L'レ'},
  {L'ろ', L'ロ'},
  {L'わ', L'ワ'},
  {L'ゐ', L'イ'},
  {L'ゑ', L'エ'},
  {L'を', L'オ'},
  {L'ん', L'ン'}
};

kana_map_t KATAKANA_MAP[] = {
  {L'ァ', L'ア'},
  {L'ィ', L'イ'},
  {L'ゥ', L'ウ'},
  {L'ェ', L'エ'},
  {L'ォ', L'オ'},
  {L'ッ', L'ツ'},
  {L'ャ', L'ヤ'},
  {L'ュ', L'ユ'},
  {L'ョ', L'ヨ'},
  {L'ヰ', L'イ'},
  {L'ヱ', L'エ'},
  {L'ヲ', L'オ'}
};
kana_map_t SYMBOL_MAP[] = {
  {L'　', L' '},
  {L'！', L'!'},
  {L'”', L'\"'},
  {L'＃', L'#'},
  {L'＄', L'$'},
  {L'％', L'%'},
  {L'＆', L'&'},
  {L'’', L'\''},
  {L'（', L'('},
  {L'）', L')'},
  {L'｛', L'{'},
  {L'｝', L'}'},
  {L'［', L'['},
  {L'］', L']'},
  {L'＜', L'<'},
  {L'＞', L'>'},
  {L'＝', L'='},
  {L'＋', L'+'},
  {L'－', L'-'},
  {L'＊', L'*'},
  {L'／', L'/'},
  {L'＼', L'\\'},
  {L'｜', L'|'},
  {L'＿', L'_'},
  {L'＠', L'@'},
  {L'：', L':'},
  {L'；', L';'},
  {L'‘', L'`'}
};
typedef struct st_hankana_map {
    wchar_t src;   /* 変換前 */
    wchar_t dest1; /* 通常変換後 */
    wchar_t dest2; /* 濁点変換後 */
    wchar_t dest3; /* 半濁点変換後 */
} hankana_map_t;

hankana_map_t HANKANA_MAP[] = {
  {L'ｱ', L'ア', L'ア', L'ア'},
  {L'ｧ', L'ア', L'ア', L'ア'},
  {L'ｲ', L'イ', L'イ', L'イ'},
  {L'ｨ', L'イ', L'イ', L'イ'},
  {L'ｳ', L'ウ', L'ウ', L'ウ'},
  {L'ｩ', L'ウ', L'ウ', L'ウ'},
  {L'ｴ', L'エ', L'エ', L'エ'},
  {L'ｪ', L'エ', L'エ', L'エ'},
  {L'ｵ', L'オ', L'オ', L'オ'},
  {L'ｫ', L'オ', L'オ', L'オ'},
  {L'ｶ', L'カ', L'ガ', L'カ'},
  {L'ｷ', L'キ', L'ギ', L'キ'},
  {L'ｸ', L'ク', L'グ', L'ク'},
  {L'ｹ', L'ケ', L'ゲ', L'ケ'},
  {L'ｺ', L'コ', L'ゴ', L'コ'},
  {L'ｻ', L'サ', L'ザ', L'サ'},
  {L'ｼ', L'シ', L'ジ', L'シ'},
  {L'ｽ', L'ス', L'ズ', L'ス'},
  {L'ｾ', L'セ', L'ゼ', L'セ'},
  {L'ｿ', L'ソ', L'ゾ', L'ソ'},
  {L'ﾀ', L'タ', L'ダ', L'タ'},
  {L'ﾁ', L'チ', L'ヂ', L'チ'},
  {L'ﾂ', L'ツ', L'ヅ', L'ツ'},
  {L'ｯ', L'ツ', L'ツ', L'ツ'},
  {L'ﾃ', L'テ', L'デ', L'テ'},
  {L'ﾄ', L'ト', L'ド', L'ト'},
  {L'ﾅ', L'ナ', L'ナ', L'ナ'},
  {L'ﾆ', L'ニ', L'ニ', L'ニ'},
  {L'ﾇ', L'ヌ', L'ヌ', L'ヌ'},
  {L'ﾈ', L'ネ', L'ネ', L'ネ'},
  {L'ﾉ', L'ノ', L'ノ', L'ノ'},
  {L'ﾊ', L'ハ', L'バ', L'パ'},
  {L'ﾋ', L'ヒ', L'ビ', L'ピ'},
  {L'ﾌ', L'フ', L'ブ', L'プ'},
  {L'ﾍ', L'ヘ', L'ベ', L'ペ'},
  {L'ﾎ', L'ホ', L'ボ', L'ポ'},
  {L'ﾏ', L'マ', L'マ', L'マ'},
  {L'ﾐ', L'ミ', L'ミ', L'ミ'},
  {L'ﾑ', L'ム', L'ム', L'ム'},
  {L'ﾒ', L'メ', L'メ', L'メ'},
  {L'ﾓ', L'モ', L'モ', L'モ'},
  {L'ﾔ', L'ヤ', L'ヤ', L'ヤ'},
  {L'ｬ', L'ヤ', L'ヤ', L'ヤ'},
  {L'ﾕ', L'ユ', L'ユ', L'ユ'},
  {L'ｭ', L'ユ', L'ユ', L'ユ'},
  {L'ﾖ', L'ヨ', L'ヨ', L'ヨ'},
  {L'ｮ', L'ヨ', L'ヨ', L'ヨ'},
  {L'ﾗ', L'ラ', L'ラ', L'ラ'},
  {L'ﾘ', L'リ', L'リ', L'リ'},
  {L'ﾙ', L'ル', L'ル', L'ル'},
  {L'ﾚ', L'レ', L'レ', L'レ'},
  {L'ﾛ', L'ロ', L'ロ', L'ロ'},
  {L'ﾜ', L'ワ', L'ワ', L'ワ'},
  {L'ｦ', L'オ', L'オ', L'オ'},
  {L'ﾝ', L'ン', L'ン', L'ン'},
  {L'､', L'，', L'，', L'，'},
  {L'｡', L'．', L'．', L'．'},
  {L'ｰ', L'ー', L'ー', L'ー'}
};

#define WC_NUMBER 101 /* 全角数字 */
#define WC_SYMBOL 102 /* 全角記号 */
#define WC_ALPHA 103 /* 全角英字 */
#define WC_HIRAGANA 201 /* 全角ひらがな */
#define WC_KATAKANA 202 /* 全角カタカナ */
#define WC_HANKANA 203 /* 半角カタカナ */
#define WC_OTHER 999 /* その他 */

int
get_wctype(wchar_t wc) {
    if ( wc >= L'ぁ' && wc <= L'ん' ) {
        return WC_HIRAGANA;
    } else if ( (wc >= L'ァ' && wc <= L'ン') || wc == L'ヴ' ) {
        return WC_KATAKANA;
    } else if ( wc >= L'０' && wc <= L'９' ) {
        return WC_NUMBER;
    } else if ( (wc >= L'Ａ' && wc <= L'Ｚ') || (wc >= L'ａ' && wc <= L'ｚ') ||
                (wc >= L'A' && wc <= L'Z') || (wc >= L'a' && wc <= L'z') ) {
        return WC_ALPHA;
    } else if ( (wc >= L'！' && wc <= L'／') ||
                (wc >= L'：' && wc <= L'＠') ||
                (wc >= L'［' && wc <= L'‘') ||
                (wc >= L'｛' && wc <= L'～') 
              ) {
        return WC_SYMBOL;
    } else if ( wc >= L'｡' && wc <= L'ﾟ' ) {
        return WC_HANKANA;
    } else {
        return WC_OTHER;
    }
}

/**
 * normalize_wc 文字毎の正規化
 * @param in
 * @param out
 **/
int
normalize_wc(wchar_t* in, wchar_t* out, offset_t* offset) {
    int wctype = get_wctype( *in );
    switch (wctype) {
        case WC_NUMBER:
            normalize_wc_number(in, out, offset);
            break;
        case WC_ALPHA:
            normalize_wc_alpha(in, out, offset);
            break;
        case WC_HIRAGANA:
            normalize_wc_hiragana(in, out, offset);
            break;
        case WC_KATAKANA:
            normalize_wc_katakana(in, out, offset);
            break;
        case WC_HANKANA:
            normalize_wc_hankana(in, out, offset);
            break;
        default:
            /* 現状はその他のパターンに記号変換を含めている。 */
            /* 記号の範囲が綺麗に取得できないから暫定的にこうしている */
            normalize_wc_symbol(in, out, offset);
            break;
    }
}

/**
 * 全角数字-> 半角数字変換
 */
int
normalize_wc_number(wchar_t* in, wchar_t* out, offset_t* offset) {
    *out = (wchar_t) ((int) *in - (int) L'０' + 0x30);
    offset->src ++;
    offset->dest ++;
}

/**
 * 全角英字-> 半角英字変換
 */
int
normalize_wc_alpha(wchar_t* in, wchar_t* out, offset_t* offset) {
    if ( *in >= L'Ａ' && *in <= L'Ｚ') {
        *out = (wchar_t) ((int) *in - (int) L'Ａ' + 0x61);
    } else if ( *in >= L'ａ' && *in <= L'ｚ') {
        *out = (wchar_t) ((int) *in - (int) L'ａ' + 0x61);
    } else if ( *in >= L'A' && *in <= L'Z') {
        *out = (wchar_t) ((int) *in - (int) L'A' + 0x61);
    } else {
        *out = *in;
    }
    offset->src ++;
    offset->dest ++;
}

/**
 * 平仮名-> カタカナ変換
 */
int
normalize_wc_hiragana(wchar_t* in, wchar_t* out, offset_t* offset) {
    int size;
    int i;
    size = sizeof(HIRAGANA_MAP) / sizeof(kana_map_t);
    for (i=0; i < size; i++) {
      if (HIRAGANA_MAP[i].src == *in) {
        *out = HIRAGANA_MAP[i].dest;
        offset->src ++;
        offset->dest ++;
        return 1;
      }
    }
    *out = *in;
    offset->src ++;
    offset->dest ++;
    return 1;
}

/**
 * 平仮名-> カタカナ変換
 * カタカナ特殊ケースの変換を行う。
 * 「ヴ」行を「バ」行に変換
 *
 * 以下は変換表で変換
 * 「ァ」行を「ア」行に変換
 * 「ャ」行を「ヤ」行に変換
 * 「ッ」を「ツ」に変換
 * 「ヰ」「ヱ」「ヲ」を「イ」「エ」「オ」に変換
 */
int
normalize_wc_katakana(wchar_t* in, wchar_t* out, offset_t* offset) {
    int size;
    int i;

    /* 「ヴ」行対応 */
    if ( *in == L'ヴ' ) {
        if ( *(in + 1) == L'ァ' ) {
            *out = L'バ';
            offset->src += 2;
            offset->dest += 1;
            return 2;
        } else if ( *(in + 1) == L'ィ' ) {
            *out = L'ビ';
            offset->src += 2;
            offset->dest += 1;
            return 2;
        } else if ( *(in + 1) == L'ェ' ) {
            *out = L'ベ';
            offset->src += 2;
            offset->dest += 1;
            return 2;
        } else if ( *(in + 1) == L'ォ' ) {
            *out = L'ボ';
            offset->src += 2;
            offset->dest += 1;
            return 2;
        } else {
            *out = L'ブ';
            offset->src += 1;
            offset->dest += 1;
            return 1;
        }
    }

    size = sizeof(KATAKANA_MAP) / sizeof(kana_map_t);
    for (i=0; i < size; i++) {
      if (KATAKANA_MAP[i].src == *in) {
        *out = KATAKANA_MAP[i].dest;
        offset->src ++;
        offset->dest ++;
        return 1;
      }
    }
    *out = *in;
    offset->src ++;
    offset->dest ++;
    return 1;
}

/**
 * 半角カタカナ->全角カタカナ変換
 * 2文字で構成される濁点・半濁点文字列は1文字の全角カタカナに変換する。
 * 不正な並びの濁点・半濁点は無視する。
 * ｳﾞｧ行の半角カタカナ文字列は1文字のバ行全角カタカナに変換する。
 * 「ｬ」「ｭ」「ｮ」「ｯ」は全角「ヤ」「ユ」「ヨ」「ツ」に変換する。
 */
normalize_wc_hankana(wchar_t* in, wchar_t* out, offset_t* offset) {
    int size;
    int i;

    /* 「ヴ」行対応 */
    if ( *in == L'ｳ' ) {
        if ( *(in + 1) == L'ﾞ' ) {
            /* ヴ行対応 */
            switch ( *(in + 2) ) {
                case L'ｧ':
                    *out = L'バ';
                    offset->src += 3;
                    offset->dest += 1;
                    return 3;
                case L'ｨ':
                    *out = L'バ';
                    offset->src += 3;
                    offset->dest += 1;
                    return 3;
                case L'ｪ':
                    *out = L'ベ';
                    offset->src += 3;
                    offset->dest += 1;
                    return 3;
                case L'ｫ':
                    *out = L'ボ';
                    offset->src += 3;
                    offset->dest += 1;
                    return 3;
                default:
                    *out = L'ブ';
                    offset->src += 2;
                    offset->dest += 1;
                    return 2;
            }
        } else {
            *out = L'ウ';
            offset->src += 1;
            offset->dest += 1;
            return 1;
        }
    }

    /* その他の半角カタカナ変換 */
    size = sizeof(HANKANA_MAP) / sizeof(hankana_map_t);
    for (i=0; i < size; i++) {
      if (HANKANA_MAP[i].src == *in) {
          if ( *(in + 1) == L'ﾞ' ) {
              /* 濁点が続く場合 */
              *out = HANKANA_MAP[i].dest2;
              offset->src += 2;
              offset->dest ++;
              return 2;
          } else if ( *(in + 1) == L'ﾞ' ) {
              /* 半濁点が続く場合 */
              *out = HANKANA_MAP[i].dest3;
              offset->src += 2;
              offset->dest ++;
              return 2;
          } else {
              /* 濁点・半濁点が続かない場合 */
              *out = HANKANA_MAP[i].dest1;
              offset->src ++;
              offset->dest ++;
              return 1;
          }
      }
    }
    *out = *in;
    offset->src ++;
    offset->dest ++;
    return 1;
}

/**
 * 全角記号-> 半角記号変換
 */
int
normalize_wc_symbol(wchar_t* in, wchar_t* out, offset_t* offset) {
    int size;
    int i;
    size = sizeof(SYMBOL_MAP) / sizeof(kana_map_t);
    for (i=0; i < size; i++) {
      if (SYMBOL_MAP[i].src == *in) {
        *out = SYMBOL_MAP[i].dest;
        offset->src ++;
        offset->dest ++;
        return 1;
      }
    }
    *out = *in;
    offset->src ++;
    offset->dest ++;
    return 1;
}

/**
 * @param org 元のワイドキャラ文字列領域
 * @param norm 正規化されたワイドキャラ文字列領域
 * @return 変換文字長
 **/
int
ntext_normalize(wchar_t* org, wchar_t* norm, int len) {
    offset_t offset = {0, 0};
    while (offset.src < len) {
#ifdef NTEXT_DEBUG
        printf("org[%d] = %lc\n", offset.src, org[offset.src]);
#endif
        normalize_wc( &org[offset.src], &norm[offset.dest], &offset);
    }
    return offset.src;
}


#ifdef NORMALIZE_UNIT_TEST
int
main(int argc, char** argv) {
    int num;
    int i;
    int len, w_len;
    setlocale( LC_CTYPE, "ja_JP.UTF-8" );
    wchar_t* org_ws;
    wchar_t* norm_ws;

    for (i=1; i < argc; i++) {
	len = strlen(argv[i]);
        printf("argv[%d]=%s\n", i, argv[i]);

	org_ws = (wchar_t*) malloc(sizeof(wchar_t) * len); 
    	w_len = mbstowcs( org_ws, argv[i], len);
	printf("org=%ls\n", org_ws);
	norm_ws = (wchar_t*) malloc(sizeof(wchar_t) * len); 

	ntext_normalize(org_ws, norm_ws, w_len);
	printf("norm=%ls\n", norm_ws);

	free(org_ws);
	free(norm_ws);
    }
}
#endif // NORMALIZE_UNIT_TEST
