#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "text_prep.h"

/* Add words SAM mispronounces here (whole words, case-insensitive). */
static const struct { const char *from, *to; } REPLACE[] = {
    { "am",   "A M" },
    { "pm",   "P M" },
    { "wifi", "why fy" },
    { "esp",  "E S P" },
};

static const char *ONES[] = {
    "zero","one","two","three","four","five","six","seven","eight","nine","ten",
    "eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen",
    "eighteen","nineteen"
};
static const char *TENS[] = {
    "","","twenty","thirty","forty","fifty","sixty","seventy","eighty","ninety"
};

typedef struct { char *o; size_t cap, len; } sb_t;

static void sb_str(sb_t *b, const char *s)
{
    while (*s && b->len < b->cap - 1) b->o[b->len++] = *s++;
    b->o[b->len] = '\0';
}
static void sb_chr(sb_t *b, char c)
{
    if (b->len < b->cap - 1) { b->o[b->len++] = c; b->o[b->len] = '\0'; }
}

static void say_below100(sb_t *b, unsigned n)
{
    if (n < 20) { sb_str(b, ONES[n]); return; }
    sb_str(b, TENS[n / 10]);
    if (n % 10) { sb_chr(b, ' '); sb_str(b, ONES[n % 10]); }
}

static void say_number(sb_t *b, unsigned n)      // 0..9999
{
    bool any = false;
    if (n >= 1000) { say_below100(b, n / 1000); sb_str(b, " thousand"); n %= 1000; any = true; }
    if (n >= 100)  { if (any) sb_chr(b, ' '); sb_str(b, ONES[n / 100]); sb_str(b, " hundred"); n %= 100; any = true; }
    if (n > 0 || !any) { if (any) sb_chr(b, ' '); say_below100(b, n); }
}

void text_prep(const char *in, char *out, size_t cap)
{
    sb_t b = { out, cap, 0 };
    out[0] = '\0';

    for (const char *p = in; *p; ) {
        if (isdigit((unsigned char)*p)) {
            char dg[16]; size_t n = 0;
            const char *q = p;
            while (isdigit((unsigned char)*q)) { if (n < 15) dg[n++] = *q; q++; }
            dg[n] = '\0';

            if (n <= 2 && *q == ':' && isdigit((unsigned char)q[1]) && isdigit((unsigned char)q[2])) {
                unsigned m = (q[1] - '0') * 10 + (q[2] - '0');
                say_number(&b, (unsigned)atoi(dg));
                if (m == 0) sb_str(&b, " o clock");
                else { sb_chr(&b, ' '); if (m < 10) sb_str(&b, "oh "); say_below100(&b, m); }
                q += 3;
            } else if (n <= 4 && (n == 1 || dg[0] != '0')) {
                say_number(&b, (unsigned)atoi(dg));
            } else {
                for (size_t i = 0; i < n; i++) { if (i) sb_chr(&b, ' '); sb_str(&b, ONES[dg[i] - '0']); }
            }
            if (isalnum((unsigned char)*q)) sb_chr(&b, ' ');
            p = q;
        } else if (isalpha((unsigned char)*p)) {
            char w[32]; size_t n = 0;
            while (isalpha((unsigned char)*p)) { if (n < 31) w[n++] = *p; p++; }
            w[n] = '\0';

            const char *rep = w;
            for (size_t i = 0; i < sizeof REPLACE / sizeof REPLACE[0]; i++) {
                if (strcasecmp(w, REPLACE[i].from) == 0) { rep = REPLACE[i].to; break; }
            }
            sb_str(&b, rep);
        } else {
            char c = *p++;
            if (c == ';' || c == ':') c = ',';
            if (c == '!') c = '.';
            sb_chr(&b, c);
        }
    }
}

