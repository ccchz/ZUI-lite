#include "mxml.h"
#include "function.h"
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#define strdup _tcsdup
#define mxml_bad_char(ch) ((ch) < _T(' ') && (ch) != _T('\n') && (ch) != _T('\r') && (ch) != _T('\t'))

/*判断字符是否为空白字符*/
static int mxml_isspace(int ch)
{
    return (ch == _T(' ') || ch == _T('\t') || ch == _T('\r') || ch == _T('\n'));
}

//--------ATTR操作
static int mxml_set_attr(mxml_node_t *node, const wchar_t *name, wchar_t *value)
{
    int i; /* Looping var */
    mxml_attr_t *attr; /* New attribute */

    for (i = node->value.num_attrs, attr = node->value.attrs;
        i > 0;
        i--, attr++)
        if (!_tcsicmp(attr->name, name))
        {
            /*
            * Free the old value as needed...
            */

            if (attr->value) {
                free(attr->value);
                attr->value = NULL;
            }
            attr->value = value;

            return (0);
        }

    if (node->value.num_attrs == 0)
        attr = (mxml_attr_t *)memset(malloc(sizeof(mxml_attr_t)), 0, sizeof(mxml_attr_t));
    else
        attr = (mxml_attr_t *)realloc(node->value.attrs,
        (node->value.num_attrs + 1) * sizeof(mxml_attr_t));

    if (!attr)
    {
        return (-1);
    }

    node->value.attrs = attr;
    attr += node->value.num_attrs;

    if ((attr->name = strdup(name)) == NULL)
    {
        return (-1);
    }

    attr->value = value;

    node->value.num_attrs++;

    return (0);
}

wchar_t* mxmlElementGetAttr(mxml_node_t *node, const wchar_t *name)
{
    int i; /* Looping var */
    mxml_attr_t *attr; /* Cirrent attribute */


    if (!node || !name)
        return (NULL);

    for (i = node->value.num_attrs, attr = node->value.attrs;
        i > 0;
        i--, attr++)
    {

        if (!_tcsicmp(attr->name, name))
        {
            return (attr->value);
        }
    }
    return (NULL);
}

void mxmlElementSetAttr(mxml_node_t *node, const wchar_t *name, const wchar_t *value)
{
    wchar_t *valuec; /* Copy of value */

    if (!node || !name)
        return;

    if (value)
        valuec = strdup(value);
    else
        valuec = NULL;

    if (mxml_set_attr(node, name, valuec))
        free(valuec);
}
//-------节点操作
void mxmlRemove(mxml_node_t *node)
{
    if (!node || !node->parent)
        return;

    /*
    * Remove from parent...
    */
    if (node->prev)
        node->prev->next = node->next;
    else
        node->parent->child = node->next;

    if (node->next)
        node->next->prev = node->prev;
    else
        node->parent->last_child = node->prev;

    node->parent = NULL;
    node->prev = NULL;
    node->next = NULL;
}

void mxmlAdd(mxml_node_t *parent, int where, mxml_node_t *child, mxml_node_t *node)
{
    if (!parent || !node)
        return;

    /*
    * Remove the node from any existing parent...
    */

    if (node->parent)
        mxmlRemove(node);

    /*
    * Reset pointers...
    */

    node->parent = parent;

    switch (where)
    {
    case MXML_ADD_BEFORE:
        if (!child || child == parent->child || child->parent != parent)
        {
            /*
            * Insert as first node under parent...
            */

            node->next = parent->child;

            if (parent->child)
                parent->child->prev = node;
            else
                parent->last_child = node;

            parent->child = node;
        }
        else
        {
            /*
            * Insert node before this child...
            */

            node->next = child;
            node->prev = child->prev;

            if (child->prev)
                child->prev->next = node;
            else
                parent->child = node;

            child->prev = node;
        }
        break;

    case MXML_ADD_AFTER:
        if (!child || child == parent->last_child || child->parent != parent)
        {
            /*
            * Insert as last node under parent...
            */

            node->parent = parent;
            node->prev = parent->last_child;

            if (parent->last_child)
                parent->last_child->next = node;
            else
                parent->child = node;

            parent->last_child = node;
        }
        else
        {
            /*
            * Insert node after this child...
            */

            node->prev = child;
            node->next = child->next;

            if (child->next)
                child->next->prev = node;
            else
                parent->last_child = node;

            child->next = node;
        }
        break;
    }
}

static mxml_node_t *mxml_new(mxml_node_t *parent)
{
    mxml_node_t *node; /* New node */

    if ((node = (mxml_node_t *)memset(malloc(sizeof(mxml_node_t)), 0, sizeof(mxml_node_t))) == NULL)
    {
        return (NULL);
    }

    if (parent)
        mxmlAdd(parent, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);
    //返回节点
    return (node);
}

void mxmlDelete(mxml_node_t *node)
{
    int i; /* Looping var */

    if (!node)
        return;

    mxmlRemove(node);

    if (node->value.num_attrs)
    {
        for (i = 0; i < node->value.num_attrs; i++)
        {
            if (node->value.attrs[i].name) {
                free(node->value.attrs[i].name);
            }
            if (node->value.attrs[i].value) {
                free(node->value.attrs[i].value);
            }
        }

        free(node->value.attrs);
        node->value.attrs = NULL;
    }

    if (node->value.name) {
        free(node->value.name);
        node->value.name = NULL;
    }

    //删除子节点
    while (node->child)
        mxmlDelete(node->child);

    //释放节点内存
    free(node);
}
mxml_node_t *mxmlClone(mxml_node_t *node, mxml_node_t *parent) {
    mxml_node_t *new_node = mxml_new(parent);
    new_node->parent = parent;
    new_node->user_data = node->user_data;
    new_node->value.name = strdup(node->value.name);
    new_node->value.num_attrs = node->value.num_attrs;
    if (node->value.num_attrs)
        new_node->value.attrs = (mxml_attr_t *)memset(malloc(node->value.num_attrs * sizeof(mxml_attr_t)), 0, node->value.num_attrs * sizeof(mxml_attr_t));
    for (int i = 0; i < node->value.num_attrs; i++)
    {
        new_node->value.attrs[i].name = strdup(node->value.attrs[i].name);
        new_node->value.attrs[i].value = strdup(node->value.attrs[i].value);
    }
    if (node->child)
        mxmlClone(node->child, new_node);
    if (parent)
        if (node->next)
            mxmlClone(node->next, parent);
    return new_node;
}
mxml_node_t *mxmlNewElement(mxml_node_t *parent, const wchar_t *name)
{
    mxml_node_t *node; /* New node */

    if (!name)
        return (NULL);

    /*
    * Create the node and set the element name...
    */

    if ((node = mxml_new(parent)) != NULL)
        node->value.name = strdup(name);

    return (node);
}

int mxmlRelease(mxml_node_t *node)
{
    if (node)
    {
        mxmlDelete(node);
        return (0);
    }
    else
        return (-1);
}
//--------XML树解析
typedef wchar_t(*mxml_getc)(mxml_buf_t *p);
//取字符
static wchar_t mxml_string_getc(mxml_buf_t *p)
{
    wchar_t ch = 0; /* Character */
    const wchar_t **s; /* Pointer to string pointer */
    s = (const wchar_t **)p;
    if (p->pos < p->len)
    {
        ch = p->buf[p->pos];


        p->pos++;
        if (ch != 0)
        {
            if (ch == 0xfeff)
                return (mxml_string_getc(p));
            if (ch == 65535)
                return (mxml_string_getc(p));
            if (mxml_bad_char(ch))
            {
                return (WEOF);
            }
            return (ch);
        }
    }
    return WEOF;
}

static int mxml_add_char(wchar_t ch, wchar_t **bufptr, wchar_t **buffer, int *bufsize)
{
    wchar_t *newbuffer; /* New buffer value */


    if (*bufptr >= (*buffer + *bufsize))
    {
        /*
        * Increase the size of the buffer...
        */

        if (*bufsize < 1024)
            (*bufsize) *= 2;
        else
            (*bufsize) += 1024;

        if ((newbuffer = (wchar_t *)realloc(*buffer, (*bufsize) * sizeof(wchar_t))) == NULL)
        {
            free(*buffer);
            *buffer = NULL;
            return (-1);
        }

        *bufptr = newbuffer + (*bufptr - *buffer);
        *buffer = newbuffer;
    }

    *(*bufptr)++ = ch;
    return (0);
}
//查找转义符
static int mxmlEntityGetValue(const wchar_t *name)
{
    int diff, /* Difference between names */
        current, /* Current entity in search */
        first, /* First entity in search */
        last; /* Last entity in search */

    static const struct
    {
        const wchar_t *name; /* Entity name */
        int val; /* Character value */
    } entities[] =
    {
    { _T("AElig"), 198 },
    { _T("Aacute"), 193 },
    { _T("Acirc"), 194 },
    { _T("Agrave"), 192 },
    { _T("Alpha"), 913 },
    { _T("Aring"), 197 },
    { _T("Atilde"), 195 },
    { _T("Auml"), 196 },
    { _T("Beta"), 914 },
    { _T("Ccedil"), 199 },
    { _T("Chi"), 935 },
    { _T("Dagger"), 8225 },
    { _T("Delta"), 916 },
    { _T("Dstrok"), 208 },
    { _T("ETH"), 208 },
    { _T("Eacute"), 201 },
    { _T("Ecirc"), 202 },
    { _T("Egrave"), 200 },
    { _T("Epsilon"), 917 },
    { _T("Eta"), 919 },
    { _T("Euml"), 203 },
    { _T("Gamma"), 915 },
    { _T("Iacute"), 205 },
    { _T("Icirc"), 206 },
    { _T("Igrave"), 204 },
    { _T("Iota"), 921 },
    { _T("Iuml"), 207 },
    { _T("Kappa"), 922 },
    { _T("Lambda"), 923 },
    { _T("Mu"), 924 },
    { _T("Ntilde"), 209 },
    { _T("Nu"), 925 },
    { _T("OElig"), 338 },
    { _T("Oacute"), 211 },
    { _T("Ocirc"), 212 },
    { _T("Ograve"), 210 },
    { _T("Omega"), 937 },
    { _T("Omicron"), 927 },
    { _T("Oslash"), 216 },
    { _T("Otilde"), 213 },
    { _T("Ouml"), 214 },
    { _T("Phi"), 934 },
    { _T("Pi"), 928 },
    { _T("Prime"), 8243 },
    { _T("Psi"), 936 },
    { _T("Rho"), 929 },
    { _T("Scaron"), 352 },
    { _T("Sigma"), 931 },
    { _T("THORN"), 222 },
    { _T("Tau"), 932 },
    { _T("Theta"), 920 },
    { _T("Uacute"), 218 },
    { _T("Ucirc"), 219 },
    { _T("Ugrave"), 217 },
    { _T("Upsilon"), 933 },
    { _T("Uuml"), 220 },
    { _T("Xi"), 926 },
    { _T("Yacute"), 221 },
    { _T("Yuml"), 376 },
    { _T("Zeta"), 918 },
    { _T("aacute"), 225 },
    { _T("acirc"), 226 },
    { _T("acute"), 180 },
    { _T("aelig"), 230 },
    { _T("agrave"), 224 },
    { _T("alefsym"), 8501 },
    { _T("alpha"), 945 },
    { _T("amp"), '&' },
    { _T("and"), 8743 },
    { _T("ang"), 8736 },
    { _T("apos"), '\'' },
    { _T("aring"), 229 },
    { _T("asymp"), 8776 },
    { _T("atilde"), 227 },
    { _T("auml"), 228 },
    { _T("bdquo"), 8222 },
    { _T("beta"), 946 },
    { _T("brkbar"), 166 },
    { _T("brvbar"), 166 },
    { _T("bull"), 8226 },
    { _T("cap"), 8745 },
    { _T("ccedil"), 231 },
    { _T("cedil"), 184 },
    { _T("cent"), 162 },
    { _T("chi"), 967 },
    { _T("circ"), 710 },
    { _T("clubs"), 9827 },
    { _T("cong"), 8773 },
    { _T("copy"), 169 },
    { _T("crarr"), 8629 },
    { _T("cup"), 8746 },
    { _T("curren"), 164 },
    { _T("dArr"), 8659 },
    { _T("dagger"), 8224 },
    { _T("darr"), 8595 },
    { _T("deg"), 176 },
    { _T("delta"), 948 },
    { _T("diams"), 9830 },
    { _T("die"), 168 },
    { _T("divide"), 247 },
    { _T("eacute"), 233 },
    { _T("ecirc"), 234 },
    { _T("egrave"), 232 },
    { _T("empty"), 8709 },
    { _T("emsp"), 8195 },
    { _T("ensp"), 8194 },
    { _T("epsilon"), 949 },
    { _T("equiv"), 8801 },
    { _T("eta"), 951 },
    { _T("eth"), 240 },
    { _T("euml"), 235 },
    { _T("euro"), 8364 },
    { _T("exist"), 8707 },
    { _T("fnof"), 402 },
    { _T("forall"), 8704 },
    { _T("frac12"), 189 },
    { _T("frac14"), 188 },
    { _T("frac34"), 190 },
    { _T("frasl"), 8260 },
    { _T("gamma"), 947 },
    { _T("ge"), 8805 },
    { _T("gt"), '>' },
    { _T("hArr"), 8660 },
    { _T("harr"), 8596 },
    { _T("hearts"), 9829 },
    { _T("hellip"), 8230 },
    { _T("hibar"), 175 },
    { _T("iacute"), 237 },
    { _T("icirc"), 238 },
    { _T("iexcl"), 161 },
    { _T("igrave"), 236 },
    { _T("image"), 8465 },
    { _T("infin"), 8734 },
    { _T("int"), 8747 },
    { _T("iota"), 953 },
    { _T("iquest"), 191 },
    { _T("isin"), 8712 },
    { _T("iuml"), 239 },
    { _T("kappa"), 954 },
    { _T("lArr"), 8656 },
    { _T("lambda"), 955 },
    { _T("lang"), 9001 },
    { _T("laquo"), 171 },
    { _T("larr"), 8592 },
    { _T("lceil"), 8968 },
    { _T("ldquo"), 8220 },
    { _T("le"), 8804 },
    { _T("lfloor"), 8970 },
    { _T("lowast"), 8727 },
    { _T("loz"), 9674 },
    { _T("lrm"), 8206 },
    { _T("lsaquo"), 8249 },
    { _T("lsquo"), 8216 },
    { _T("lt"), '<' },
    { _T("macr"), 175 },
    { _T("mdash"), 8212 },
    { _T("micro"), 181 },
    { _T("middot"), 183 },
    { _T("minus"), 8722 },
    { _T("mu"), 956 },
    { _T("nabla"), 8711 },
    { _T("nbsp"), 160 },
    { _T("ndash"), 8211 },
    { _T("ne"), 8800 },
    { _T("ni"), 8715 },
    { _T("not"), 172 },
    { _T("notin"), 8713 },
    { _T("nsub"), 8836 },
    { _T("ntilde"), 241 },
    { _T("nu"), 957 },
    { _T("oacute"), 243 },
    { _T("ocirc"), 244 },
    { _T("oelig"), 339 },
    { _T("ograve"), 242 },
    { _T("oline"), 8254 },
    { _T("omega"), 969 },
    { _T("omicron"), 959 },
    { _T("oplus"), 8853 },
    { _T("or"), 8744 },
    { _T("ordf"), 170 },
    { _T("ordm"), 186 },
    { _T("oslash"), 248 },
    { _T("otilde"), 245 },
    { _T("otimes"), 8855 },
    { _T("ouml"), 246 },
    { _T("para"), 182 },
    { _T("part"), 8706 },
    { _T("permil"), 8240 },
    { _T("perp"), 8869 },
    { _T("phi"), 966 },
    { _T("pi"), 960 },
    { _T("piv"), 982 },
    { _T("plusmn"), 177 },
    { _T("pound"), 163 },
    { _T("prime"), 8242 },
    { _T("prod"), 8719 },
    { _T("prop"), 8733 },
    { _T("psi"), 968 },
    { _T("quot"), '\"' },
    { _T("rArr"), 8658 },
    { _T("radic"), 8730 },
    { _T("rang"), 9002 },
    { _T("raquo"), 187 },
    { _T("rarr"), 8594 },
    { _T("rceil"), 8969 },
    { _T("rdquo"), 8221 },
    { _T("real"), 8476 },
    { _T("reg"), 174 },
    { _T("rfloor"), 8971 },
    { _T("rho"), 961 },
    { _T("rlm"), 8207 },
    { _T("rsaquo"), 8250 },
    { _T("rsquo"), 8217 },
    { _T("sbquo"), 8218 },
    { _T("scaron"), 353 },
    { _T("sdot"), 8901 },
    { _T("sect"), 167 },
    { _T("shy"), 173 },
    { _T("sigma"), 963 },
    { _T("sigmaf"), 962 },
    { _T("sim"), 8764 },
    { _T("spades"), 9824 },
    { _T("sub"), 8834 },
    { _T("sube"), 8838 },
    { _T("sum"), 8721 },
    { _T("sup"), 8835 },
    { _T("sup1"), 185 },
    { _T("sup2"), 178 },
    { _T("sup3"), 179 },
    { _T("supe"), 8839 },
    { _T("szlig"), 223 },
    { _T("tau"), 964 },
    { _T("there4"), 8756 },
    { _T("theta"), 952 },
    { _T("thetasym"), 977 },
    { _T("thinsp"), 8201 },
    { _T("thorn"), 254 },
    { _T("tilde"), 732 },
    { _T("times"), 215 },
    { _T("trade"), 8482 },
    { _T("uArr"), 8657 },
    { _T("uacute"), 250 },
    { _T("uarr"), 8593 },
    { _T("ucirc"), 251 },
    { _T("ugrave"), 249 },
    { _T("uml"), 168 },
    { _T("upsih"), 978 },
    { _T("upsilon"), 965 },
    { _T("uuml"), 252 },
    { _T("weierp"), 8472 },
    { _T("xi"), 958 },
    { _T("yacute"), 253 },
    { _T("yen"), 165 },
    { _T("yuml"), 255 },
    { _T("zeta"), 950 },
    { _T("zwj"), 8205 },
    { _T("zwnj"), 8204 }
    };


    /*
    * Do a binary search for the named entity...
    */

    first = 0;
    last = (int)(sizeof(entities) / sizeof(entities[0]) - 1);

    while ((last - first) > 1)
    {
        current = (first + last) / 2;

        if ((diff = _tcsicmp(name, entities[current].name)) == 0)
            return (entities[current].val);
        else if (diff < 0)
            last = current;
        else
            first = current;
    }

    /*
    * If we get here, there is a small chance that there is still
    * a match; check first and last...
    */

    if (!_tcsicmp(name, entities[first].name))
        return (entities[first].val);
    else if (!_tcsicmp(name, entities[last].name))
        return (entities[last].val);
    else
        return (-1);
}
//处理转义符号
static int mxml_get_entity(mxml_node_t *parent, void *p, mxml_getc mxml_string_getc)
{
    int ch; /* Current character */
    wchar_t entity[64], /* Entity string */
        *entptr; /* Pointer into entity */

    entptr = entity;

    while ((ch = mxml_string_getc(p)) != WEOF)
        if (ch > 126 || (!iswalnum(ch) && ch != _T('#')))
            break;
        else if (entptr < (entity + sizeof(entity) - 1))
            *entptr++ = ch;
        else
        {
            break;
        }

    *entptr = _T('\0');

    if (ch != _T(';'))
    {
        return (WEOF);
    }

    if (entity[0] == _T('#'))
    {
        if (entity[1] == _T('x'))
            ch = _tcstol(entity + 2, NULL, 16);
        else
            ch = _tcstol(entity + 1, NULL, 10);
    }
    else
        ch = mxmlEntityGetValue(entity);

    if (mxml_bad_char(ch))
    {
        return (WEOF);
    }

    return (ch);
}

static int mxml_parse_element(mxml_node_t *node, void *p, mxml_getc mxml_string_getc)
{
    wchar_t ch, /* Current character in file */
        quote; /* Quoting character */
    wchar_t *name, /* Attribute name */
        *value, /* Attribute value */
        *ptr; /* Pointer into name/value */
    int namesize, /* Size of name string */
        valsize; /* Size of value string */


        /*
        * Initialize the name and value buffers...
        */

    if ((name = (wchar_t *)memset(malloc(64 * sizeof(wchar_t)), 0, 64 * sizeof(wchar_t))) == NULL)
    {
        return (WEOF);
    }

    namesize = 64;

    if ((value = (wchar_t *)memset(malloc(64 * sizeof(wchar_t)), 0, 64 * sizeof(wchar_t))) == NULL)
    {
        free(name);
        return (WEOF);
    }

    valsize = 64;

    /*
    * Loop until we hit a >, /, ?, or EOF...
    */

    while ((ch = mxml_string_getc(p)) != WEOF)
    {
        if (mxml_isspace(ch))
            continue;

        /*
        * Stop at /, ?, or >...
        */

        if (ch == _T('/') || ch == _T('?'))
        {
            /*
            * Grab the > character and print an error if it isn't there...
            */

            quote = mxml_string_getc(p);

            if (quote != _T('>'))
            {
                goto error;
            }

            break;
        }
        else if (ch == _T('<'))
        {
            goto error;
        }
        else if (ch == _T('>'))
            break;

        /*
        * Read the attribute name...
        */


        if (ch == _T('\"') || ch == _T('\''))
        {
            /*
            * Name is in quotes, so get a quoted string...
            */

            quote = ch;
            ptr = name;

            while ((ch = mxml_string_getc(p)) != WEOF)
            {
                if (ch == quote) {
                    ch = mxml_string_getc(p);
                    break;
                }
                if (ch == _T('&')) {
                    if ((ch = mxml_get_entity(node, p, mxml_string_getc)) == WEOF)
                        goto error;
                    if (ch == _T(';'))
                        continue;//跳过分号
                }
                if (mxml_add_char(ch, &ptr, &name, &namesize))
                    goto error;

            }
        }
        else
        {
            /*
            * Grab an normal, non-quoted name...
            */

            name[0] = ch;
            ptr = name + 1;
            while ((ch = mxml_string_getc(p)) != WEOF)
                if (mxml_isspace(ch) || ch == _T('=') || ch == _T('/') || ch == _T('>') ||
                    ch == _T('?'))
                    break;
                else
                {
                    if (ch == _T('&')) {
                        if ((ch = mxml_get_entity(node, p, mxml_string_getc)) == WEOF)
                            goto error;
                        if (ch == ';')
                            continue;//跳过分号
                    }
                    if (mxml_add_char(ch, &ptr, &name, &namesize))
                        goto error;
                }
        }

        *ptr = _T('\0');

        if (mxmlElementGetAttr(node, name))
            goto error;

        while (ch != WEOF && mxml_isspace(ch))
            ch = mxml_string_getc(p);

        if (ch == _T('='))
        {
            /*
            * Read the attribute value...
            */

            while ((ch = mxml_string_getc(p)) != WEOF && mxml_isspace(ch));

            if (ch == WEOF)
            {
                goto error;
            }

            if (ch == _T('\'') || ch == _T('\"'))
            {
                /*
                * Read quoted value...
                */

                quote = ch;
                ptr = value;

                while ((ch = mxml_string_getc(p)) != WEOF)
                    if (ch == quote)
                        break;
                    else
                    {
                        if (ch == _T('&')) {
                            if ((ch = mxml_get_entity(node, p, mxml_string_getc)) == WEOF)
                                goto error;
                            if (ch == _T(';'))
                                continue;//跳过分号
                        }
                        if (mxml_add_char(ch, &ptr, &value, &valsize))
                            goto error;
                    }

                *ptr = _T('\0');
            }
            else
            {
                /*
                * Read unquoted value...
                */

                value[0] = ch;
                ptr = value + 1;

                while ((ch = mxml_string_getc(p)) != WEOF)
                    if (mxml_isspace(ch) || ch == _T('=') || ch ==  _T('/')|| ch == _T('>'))
                        break;
                    else
                    {
                        if (ch == _T('&')) {
                            if ((ch = mxml_get_entity(node, p, mxml_string_getc)) == WEOF)
                                goto error;
                            if (ch == _T(';'))
                                continue;//跳过分号
                        }
                        if (mxml_add_char(ch, &ptr, &value, &valsize))
                            goto error;
                    }

                *ptr = _T('\0');
            }

            /*
            * Set the attribute with the given string value...
            */

            mxmlElementSetAttr(node, name, value);
        }
        else
        {
            goto error;
        }

        /*
        * Check the end character...
        */

        if (ch == _T('/') || ch == _T('?'))
        {
            /*
            * Grab the > character and print an error if it isn't there...
            */

            quote = mxml_string_getc(p);

            if (quote != _T('>'))
            {
                ch = WEOF;
            }

            break;
        }
        else if (ch == _T('>'))
            break;
    }

    /*
    * Free the name and value buffers and return...
    */

    free(name);
    free(value);

    return (ch);

error:

    free(name);
    free(value);

    return (WEOF);
}
//加载XML字符串 返回XML树
mxml_node_t *mxmlLoadString(mxml_node_t *top, ZPARAM s, int len)
{
    mxml_node_t *node, /* Current node */
        *first, /* First node added */
        *parent; /* Current parent node */
    wchar_t ch; /* Non-zero if whitespace seen */
    wchar_t *buffer, /* String buffer */
        *bufptr; /* Pointer into buffer */
    size_t bufsize = 0; /* Size of buffer */
    wchar_t *txtbuf = 0;
    if (ZuiStingIsUtf8(s, len))
    {
        bufsize = ZuiUtf8ToUnicode(s, len, 0, 0) * sizeof(wchar_t);
        txtbuf = malloc(bufsize + 2);
        bufsize = ZuiUtf8ToUnicode(s, len, txtbuf, bufsize);
        txtbuf[bufsize] = 0;
    }
    else
    {
        bufsize = ZuiAsciiToUnicode(s, len, 0, 0) * sizeof(wchar_t);
        txtbuf = malloc(bufsize + 2);
        bufsize = ZuiAsciiToUnicode(s, len, txtbuf, bufsize);
        txtbuf[bufsize] = 0;
    }
    mxml_buf_t buf;
    buf.buf = txtbuf;
    buf.len = bufsize;
    buf.pos = 0;
    mxml_buf_t *p = &buf;
    if ((buffer = (wchar_t *)malloc(64 * sizeof(wchar_t))) == NULL)
        return (NULL);
    bufsize = 64 * sizeof(wchar_t);
    bufptr = buffer;
    parent = top;
    first = NULL;

    while ((ch = mxml_string_getc(p)) != WEOF)
    {
        if ((ch == _T('<') || mxml_isspace(ch)) && bufptr > buffer)
        {
            *bufptr = _T('\0');

            node = mxmlNewElement(parent, buffer);

            if (*bufptr)
            {
                break;
            }

            bufptr = buffer;

            if (!first && node)
                first = node;
        }

        if (ch == _T('<'))
        {
            bufptr = buffer;

            while ((ch = mxml_string_getc(p)) != WEOF)
                if (mxml_isspace(ch) || ch == _T('>') || (ch == _T('/') && bufptr > buffer))
                    break;
                else if (ch == _T('<'))
                {
                    goto error;
                }
                else if (ch == _T('&'))
                {
                    if ((ch = mxml_get_entity(parent, p, mxml_string_getc)) == WEOF)
                        goto error;
                    if (ch == _T(';'))
                        continue;//跳过分号
                    if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                        goto error;
                }
                else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                    goto error;
                else if (((bufptr - buffer) == 1 && buffer[0] == _T('?')) ||
                    ((bufptr - buffer) == 3 && !_tcsncmp(buffer, _T("!--"), 3)) ||
                    ((bufptr - buffer) == 8 && !_tcsncmp(buffer, _T("![CDATA["), 8)))
                    break;

            *bufptr = _T('\0');

            if (!_tcsicmp(buffer, _T("!--")))
            {
                while ((ch = mxml_string_getc(p)) != WEOF)
                {
                    if (ch == _T('>') && bufptr > (buffer + 4) &&
                        bufptr[-3] != _T('-') && bufptr[-2] == _T('-') && bufptr[-1] == _T('-'))
                        break;
                    else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                        goto error;
                }

                if (ch != _T('>'))
                {
                    goto error;
                }

                *bufptr = _T('\0');

                if (!parent && first)
                {
                    goto error;
                }
                //不解析跳过注释
                node = NULL;
            }
            else if (!_tcsicmp(buffer, _T("![CDATA[")))
            {
                while ((ch = mxml_string_getc(p)) != WEOF)
                {
                    if (ch == _T('>') && !_tcsncmp(bufptr - 2, _T("]]"), 2))
                        break;
                    else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                        goto error;
                }

                if (ch != _T('>'))
                {
                    goto error;
                }

                *bufptr = '\0';

                if (!parent && first)
                {
                    goto error;
                }

                if ((node = mxmlNewElement(parent, buffer)) == NULL)
                {
                    goto error;
                }

                if (node && !first)
                    first = node;
            }
            else if (buffer[0] == _T('?'))
            {
                while ((ch = mxml_string_getc(p)) != WEOF)
                {
                    if (ch == _T('>') && bufptr > buffer && bufptr[-1] == _T('?'))
                        break;
                    else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                        goto error;
                }
                if (ch != _T('>'))
                {
                    goto error;
                }

                *bufptr = _T('\0');

                if (!parent && first)
                {
                    goto error;
                }

                if ((node = mxmlNewElement(parent, buffer)) == NULL)
                {
                    goto error;
                }

                if (node)
                {
                    if (!first)
                        first = node;

                    if (!parent)
                    {
                        parent = node;
                    }
                }
            }
            else if (buffer[0] == _T('!'))
            {
                do
                {
                    if (ch == _T('>'))
                        break;
                    else
                    {
                        if (ch == _T('&')) {
                            if ((ch = mxml_get_entity(parent, p, mxml_string_getc)) == WEOF)
                                goto error;
                            if (ch == _T(';'))
                                continue;//跳过分号
                        }
                        if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                            goto error;
                    }
                } while ((ch = mxml_string_getc(p)) != WEOF);

                if (ch != _T('>'))
                {
                    goto error;
                }

                *bufptr = _T('\0');

                if (!parent && first)
                {
                    goto error;
                }

                if ((node = mxmlNewElement(parent, buffer)) == NULL)
                {
                    goto error;
                }

                if (node)
                {
                    if (!first)
                        first = node;

                    if (!parent)
                    {
                        parent = node;
                    }
                }
            }
            else if (buffer[0] == _T('/'))
            {
                if (!parent || _tcsicmp(buffer + 1, parent->value.name))
                {
                    goto error;
                }

                while (ch != _T('>') && ch != WEOF)
                    ch = mxml_string_getc(p);

                node = parent;
                parent = parent->parent;
            }
            else
            {
                if (!parent && first)
                {
                    goto error;
                }

                if ((node = mxmlNewElement(parent, buffer)) == NULL)
                {
                    goto error;
                }

                if (mxml_isspace(ch))
                {
                    if ((ch = mxml_parse_element(node, p, mxml_string_getc)) == WEOF)
                        goto error;
                }
                else if (ch == _T('/'))
                {
                    if ((ch = mxml_string_getc(p)) != _T('>'))
                    {
                        mxmlDelete(node);
                        goto error;
                    }

                    ch = _T('/');
                }

                if (!first)
                    first = node;

                if (ch == WEOF)
                    break;

                if (ch != _T('/'))
                {
                    parent = node;
                }
            }

            bufptr = buffer;
        }
        else if (ch == _T('&'))
        {
            if ((ch = mxml_get_entity(parent, p, mxml_string_getc)) == WEOF)
                goto error;
            if (ch == _T(';'))
                continue;//跳过分号
            if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                goto error;
        }
        else if (!mxml_isspace(ch))
        {
            if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
                goto error;
        }
    }

    free(buffer);
    buffer = NULL;
    if (txtbuf) {
        free(txtbuf);
        txtbuf = NULL;
    }
    if (parent)
    {
        node = parent;

        while (parent != top && parent->parent)
            parent = parent->parent;

        if (node != parent)
        {
            mxmlDelete(first);
            return (NULL);
        }
    }
    if (parent)
        return (parent);
    else
        return (first);
error:

    mxmlDelete(first);
    free(buffer);
    return (NULL);
}
//-------节点查找
mxml_node_t *mxmlFindElement(mxml_node_t *node, mxml_node_t *top, const wchar_t *name, const wchar_t *attr, const wchar_t *value, int descend)
{
    wchar_t* temp; /* Current attribute value */


    /*
    * Range check input...
    */

    if (!node || !top || (!attr && value))
        return (NULL);

    /*
    * Start with the next node...
    */

    node = mxmlWalkNext(node, top, descend);

    /*
    * Loop until we find a matching element...
    */

    while (node != NULL)
    {
        /*
        * See if this node matches...
        */

        if (node->value.name &&
            (!name || !_tcsicmp(node->value.name, name)))
        {
            /*
            * See if we need to check for an attribute...
            */

            if (!attr)
                return (node); /* No attribute search, return it... */

                /*
                * Check for the attribute...
                */

            if ((temp = mxmlElementGetAttr(node, attr)) != NULL)
            {
                /*
                * OK, we have the attribute, does it match?
                */

                if (!value || !_tcsicmp(value, temp))
                    return (node); /* Yes, return it... */
            }
        }

        /*
        * No match, move on to the next node...
        */

        if (descend == MXML_DESCEND)
            node = mxmlWalkNext(node, top, MXML_DESCEND);
        else
            node = node->next;
    }

    return (NULL);
}

mxml_node_t *mxmlFindPath(mxml_node_t *top, const wchar_t *path)
{
    mxml_node_t *node; /* Current node */
    wchar_t element[256]; /* Current element name */
    const wchar_t *pathsep; /* Separator in path */
    int descend; /* mxmlFindElement option */


    /*
    * Range check input...
    */

    if (!top || !path || !*path)
        return (NULL);

    /*
    * Search each element in the path...
    */

    node = top;
    while (*path)
    {
        /*
        * Handle wildcards...
        */

        if (!_tcsncmp(path, _T("*/"), 2))
        {
            path += 2;
            descend = MXML_DESCEND;
        }
        else
            descend = MXML_DESCEND_FIRST;

        /*
        * Get the next element in the path...
        */

        if ((pathsep = _tcschr(path, _T('/'))) == NULL)
            pathsep = path + _tcslen(path);

        if (pathsep == path || (pathsep - path) >= sizeof(element))
            return (NULL);

        memcpy(element, path, (pathsep - path)*sizeof(wchar_t));
        element[pathsep - path] = '\0';

        if (*pathsep)
            path = pathsep + 1;
        else
            path = pathsep;

        /*
        * Search for the element...
        */

        if ((node = mxmlFindElement(node, node, element, NULL, NULL,
            descend)) == NULL)
            return (NULL);
    }

    /*
    * If we get this far, return the node or its first child...
    */

    if (node->child)
        return (node->child);
    else
        return (node);
}

mxml_node_t *mxmlWalkNext(mxml_node_t *node, mxml_node_t *top, int descend)
{
    if (!node)
        return (NULL);
    else if (node->child && descend)
        return (node->child);
    else if (node == top)
        return (NULL);
    else if (node->next)
        return (node->next);
    else if (node->parent && node->parent != top)
    {
        node = node->parent;

        while (!node->next)
            if (node->parent == top || !node->parent)
                return (NULL);
            else
                node = node->parent;

        return (node->next);
    }
    else
        return (NULL);
}

mxml_node_t *mxmlWalkPrev(mxml_node_t *node, mxml_node_t *top, int descend)
{
    if (!node || node == top)
        return (NULL);
    else if (node->prev)
    {
        if (node->prev->last_child && descend)
        {
            /*
            * Find the last child under the previous node...
            */

            node = node->prev->last_child;

            while (node->last_child)
                node = node->last_child;

            return (node);
        }
        else
            return (node->prev);
    }
    else if (node->parent != top)
        return (node->parent);
    else
        return (NULL);
}

