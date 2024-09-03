namespace YY::Thunks
{
#if defined(YY_Thunks_Implemented) && (YY_Thunks_Target < __WindowsNT6)
    namespace internal
    {
        namespace
        {
            //逆向来自微软的Nlsdl.dll，获取一个区域的 Fallback区域
            static LCID __fastcall DownlevelGetFallbackLocaleLCID(LCID Locale)
            {
                struct LcidFallback
                {
                    LCID Locale;
                    LCID Fallback;
                };

                static constexpr LcidFallback FallbackTable[] =
                {
                    { 0x00000004, 0x00007804 }, //zh-Hans -> zh
                    { 0x00000401, 0x00000001 }, //ar-SA -> ar 
                    { 0x00000402, 0x00000002 }, //bg-BG -> bg 
                    { 0x00000403, 0x00000003 }, //ca-ES -> ca 
                    { 0x00000404, 0x00007C04 }, //zh-TW -> zh-Hant 
                    { 0x00000405, 0x00000005 }, //cs-CZ -> cs 
                    { 0x00000406, 0x00000006 }, //da-DK -> da 
                    { 0x00000407, 0x00000007 }, //de-DE -> de 
                    { 0x00000408, 0x00000008 }, //el-GR -> el 
                    { 0x00000409, 0x00000009 }, //en-US -> en 
                    { 0x0000040A, 0x0000000A }, //es-ES_tradnl -> es 
                    { 0x0000040B, 0x0000000B }, //fi-FI -> fi 
                    { 0x0000040C, 0x0000000C }, //fr-FR -> fr 
                    { 0x0000040D, 0x0000000D }, //he-IL -> he 
                    { 0x0000040E, 0x0000000E }, //hu-HU -> hu 
                    { 0x0000040F, 0x0000000F }, //is-IS -> is 
                    { 0x00000410, 0x00000010 }, //it-IT -> it 
                    { 0x00000411, 0x00000011 }, //ja-JP -> ja 
                    { 0x00000412, 0x00000012 }, //ko-KR -> ko 
                    { 0x00000413, 0x00000013 }, //nl-NL -> nl 
                    { 0x00000414, 0x00007C14 }, //nb-NO -> nb 
                    { 0x00000415, 0x00000015 }, //pl-PL -> pl 
                    { 0x00000416, 0x00000016 }, //pt-BR -> pt 
                    { 0x00000417, 0x00000017 }, //rm-CH -> rm 
                    { 0x00000418, 0x00000018 }, //ro-RO -> ro 
                    { 0x00000419, 0x00000019 }, //ru-RU -> ru 
                    { 0x0000041A, 0x0000001A }, //hr-HR -> hr 
                    { 0x0000041B, 0x0000001B }, //sk-SK -> sk 
                    { 0x0000041C, 0x0000001C }, //sq-AL -> sq 
                    { 0x0000041D, 0x0000001D }, //sv-SE -> sv 
                    { 0x0000041E, 0x0000001E }, //th-TH -> th 
                    { 0x0000041F, 0x0000001F }, //tr-TR -> tr 
                    { 0x00000420, 0x00000020 }, //ur-PK -> ur 
                    { 0x00000421, 0x00000021 }, //id-ID -> id 
                    { 0x00000422, 0x00000022 }, //uk-UA -> uk 
                    { 0x00000423, 0x00000023 }, //be-BY -> be 
                    { 0x00000424, 0x00000024 }, //sl-SI -> sl 
                    { 0x00000425, 0x00000025 }, //et-EE -> et 
                    { 0x00000426, 0x00000026 }, //lv-LV -> lv 
                    { 0x00000427, 0x00000027 }, //lt-LT -> lt 
                    { 0x00000428, 0x00007C28 }, //tg-Cyrl-TJ -> tg-Cyrl
                    { 0x00000429, 0x00000029 }, //fa-IR -> fa 
                    { 0x0000042A, 0x0000002A }, //vi-VN -> vi 
                    { 0x0000042B, 0x0000002B }, //hy-AM -> hy 
                    { 0x0000042C, 0x0000782C }, //az-Latn-AZ -> az-Latn
                    { 0x0000042D, 0x0000002D }, //eu-ES -> eu 
                    { 0x0000042E, 0x0000002E }, //hsb-DE -> hsb 
                    { 0x0000042F, 0x0000002F }, //mk-MK -> mk 
                    { 0x00000432, 0x00000032 }, //tn-ZA -> tn 
                    { 0x00000434, 0x00000034 }, //xh-ZA -> xh 
                    { 0x00000435, 0x00000035 }, //zu-ZA -> zu 
                    { 0x00000436, 0x00000036 }, //af-ZA -> af 
                    { 0x00000437, 0x00000037 }, //ka-GE -> ka 
                    { 0x00000438, 0x00000038 }, //fo-FO -> fo 
                    { 0x00000439, 0x00000039 }, //hi-IN -> hi 
                    { 0x0000043A, 0x0000003A }, //mt-MT -> mt 
                    { 0x0000043B, 0x0000003B }, //se-NO -> se 
                    { 0x0000043E, 0x0000003E }, //ms-MY -> ms 
                    { 0x0000043F, 0x0000003F }, //kk-KZ -> kk 
                    { 0x00000440, 0x00000040 }, //ky-KG -> ky 
                    { 0x00000441, 0x00000041 }, //sw-KE -> sw 
                    { 0x00000442, 0x00000042 }, //tk-TM -> tk 
                    { 0x00000443, 0x00007C43 }, //uz-Latn-UZ -> uz-Latn
                    { 0x00000444, 0x00000044 }, //tt-RU -> tt 
                    { 0x00000445, 0x00000045 }, //bn-IN -> bn 
                    { 0x00000446, 0x00000046 }, //pa-IN -> pa 
                    { 0x00000447, 0x00000047 }, //gu-IN -> gu 
                    { 0x00000448, 0x00000048 }, //or-IN -> or 
                    { 0x00000449, 0x00000049 }, //ta-IN -> ta 
                    { 0x0000044A, 0x0000004A }, //te-IN -> te 
                    { 0x0000044B, 0x0000004B }, //kn-IN -> kn 
                    { 0x0000044C, 0x0000004C }, //ml-IN -> ml 
                    { 0x0000044D, 0x0000004D }, //as-IN -> as 
                    { 0x0000044E, 0x0000004E }, //mr-IN -> mr 
                    { 0x0000044F, 0x0000004F }, //sa-IN -> sa 
                    { 0x00000450, 0x00007850 }, //mn-MN -> mn-Cyrl
                    { 0x00000451, 0x00000051 }, //bo-CN -> bo 
                    { 0x00000452, 0x00000052 }, //cy-GB -> cy 
                    { 0x00000453, 0x00000053 }, //km-KH -> km 
                    { 0x00000454, 0x00000054 }, //lo-LA -> lo 
                    { 0x00000456, 0x00000056 }, //gl-ES -> gl 
                    { 0x00000457, 0x00000057 }, //kok-IN -> kok 
                    { 0x0000045A, 0x0000005A }, //syr-SY -> syr 
                    { 0x0000045B, 0x0000005B }, //si-LK -> si 
                    { 0x0000045D, 0x0000785D }, //iu-Cans-CA -> iu-Cans
                    { 0x0000045E, 0x0000005E }, //am-ET -> am 
                    { 0x00000461, 0x00000061 }, //ne-NP -> ne 
                    { 0x00000462, 0x00000062 }, //fy-NL -> fy 
                    { 0x00000463, 0x00000063 }, //ps-AF -> ps 
                    { 0x00000464, 0x00000064 }, //fil-PH -> fil 
                    { 0x00000465, 0x00000065 }, //dv-MV -> dv 
                    { 0x00000468, 0x00007C68 }, //ha-Latn-NG -> ha-Latn
                    { 0x0000046A, 0x0000006A }, //yo-NG -> yo 
                    { 0x0000046B, 0x0000006B }, //quz-BO -> quz 
                    { 0x0000046C, 0x0000006C }, //nso-ZA -> nso 
                    { 0x0000046D, 0x0000006D }, //ba-RU -> ba 
                    { 0x0000046E, 0x0000006E }, //lb-LU -> lb 
                    { 0x0000046F, 0x0000006F }, //kl-GL -> kl 
                    { 0x00000470, 0x00000070 }, //ig-NG -> ig 
                    { 0x00000478, 0x00000078 }, //ii-CN -> ii 
                    { 0x0000047A, 0x0000007A }, //arn-CL -> arn 
                    { 0x0000047C, 0x0000007C }, //moh-CA -> moh 
                    { 0x0000047E, 0x0000007E }, //br-FR -> br 
                    { 0x00000480, 0x00000080 }, //ug-CN -> ug 
                    { 0x00000481, 0x00000081 }, //mi-NZ -> mi 
                    { 0x00000482, 0x00000082 }, //oc-FR -> oc 
                    { 0x00000483, 0x00000083 }, //co-FR -> co 
                    { 0x00000484, 0x00000084 }, //gsw-FR -> gsw 
                    { 0x00000485, 0x00000085 }, //sah-RU -> sah 
                    { 0x00000486, 0x00000086 }, //qut-GT -> qut 
                    { 0x00000487, 0x00000087 }, //rw-RW -> rw 
                    { 0x00000488, 0x00000088 }, //wo-SN -> wo 
                    { 0x0000048C, 0x0000008C }, //prs-AF -> prs 
                    { 0x00000491, 0x00000091 }, //gd-GB -> gd 
                    { 0x00000501, 0x00000009 }, //qps-ploc -> en 
                    { 0x000005FE, 0x00000011 }, //qps-ploca -> ja 
                    { 0x00000801, 0x00000001 }, //ar-IQ -> ar 
                    { 0x00000804, 0x00000004 }, //zh-CN -> zh-Hans 
                    { 0x00000807, 0x00000007 }, //de-CH -> de 
                    { 0x00000809, 0x00000009 }, //en-GB -> en 
                    { 0x0000080A, 0x0000000A }, //es-MX -> es 
                    { 0x0000080C, 0x0000000C }, //fr-BE -> fr 
                    { 0x00000810, 0x00000010 }, //it-CH -> it 
                    { 0x00000813, 0x00000013 }, //nl-BE -> nl 
                    { 0x00000814, 0x00007814 }, //nn-NO -> nn 
                    { 0x00000816, 0x00000016 }, //pt-PT -> pt 
                    { 0x0000081A, 0x0000701A }, //sr-Latn-CS -> sr-Latn 
                    { 0x0000081D, 0x0000001D }, //sv-FI -> sv 
                    { 0x0000082C, 0x0000742C }, //az-Cyrl-AZ -> az-Cyrl 
                    { 0x0000082E, 0x00007C2E }, //dsb-DE -> dsb 
                    { 0x0000083B, 0x0000003B }, //se-SE -> se 
                    { 0x0000083C, 0x0000003C }, //ga-IE -> ga 
                    { 0x0000083E, 0x0000003E }, //ms-BN -> ms 
                    { 0x00000843, 0x00007843 }, //uz-Cyrl-UZ -> uz-Cyrl
                    { 0x00000845, 0x00000045 }, //bn-BD -> bn 
                    { 0x00000850, 0x00007C50 }, //mn-Mong-CN -> mn-Mong
                    { 0x0000085D, 0x00007C5D }, //iu-Latn-CA -> iu-Latn
                    { 0x0000085F, 0x00007C5F }, //tzm-Latn-DZ -> tzm-Latn
                    { 0x0000086B, 0x0000006B }, //quz-EC -> quz 
                    { 0x000009FF, 0x00000001 }, //qps-plocm -> ar 
                    { 0x00000C01, 0x00000001 }, //ar-EG -> ar 
                    { 0x00000C04, 0x00007C04 }, //zh-HK -> zh-Hant 
                    { 0x00000C07, 0x00000007 }, //de-AT -> de 
                    { 0x00000C09, 0x00000009 }, //en-AU -> en 
                    { 0x00000C0A, 0x0000000A }, //es-ES -> es 
                    { 0x00000C0C, 0x0000000C }, //fr-CA -> fr 
                    { 0x00000C1A, 0x00006C1A }, //sr-Cyrl-CS -> sr-Cyrl
                    { 0x00000C3B, 0x0000003B }, //se-FI -> se 
                    { 0x00000C6B, 0x0000006B }, //quz-PE -> quz 
                    { 0x00001001, 0x00000001 }, //ar-LY -> ar 
                    { 0x00001004, 0x00000004 }, //zh-SG -> zh-Hans 
                    { 0x00001007, 0x00000007 }, //de-LU -> de 
                    { 0x00001009, 0x00000009 }, //en-CA -> en 
                    { 0x0000100A, 0x0000000A }, //es-GT -> es 
                    { 0x0000100C, 0x0000000C }, //fr-CH -> fr 
                    { 0x0000101A, 0x0000001A }, //hr-BA -> hr 
                    { 0x0000103B, 0x00007C3B }, //smj-NO -> smj 
                    { 0x00001401, 0x00000001 }, //ar-DZ -> ar 
                    { 0x00001404, 0x00007C04 }, //zh-MO -> zh-Hant 
                    { 0x00001407, 0x00000007 }, //de-LI -> de 
                    { 0x00001409, 0x00000009 }, //en-NZ -> en 
                    { 0x0000140A, 0x0000000A }, //es-CR -> es 
                    { 0x0000140C, 0x0000000C }, //fr-LU -> fr 
                    { 0x0000141A, 0x0000681A }, //bs-Latn-BA -> bs-Latn
                    { 0x0000143B, 0x00007C3B }, //smj-SE -> smj 
                    { 0x00001801, 0x00000001 }, //ar-MA -> ar 
                    { 0x00001809, 0x00000009 }, //en-IE -> en 
                    { 0x0000180A, 0x0000000A }, //es-PA -> es 
                    { 0x0000180C, 0x0000000C }, //fr-MC -> fr 
                    { 0x0000181A, 0x0000701A }, //sr-Latn-BA -> sr-Latn
                    { 0x0000183B, 0x0000783B }, //sma-NO -> sma 
                    { 0x00001C01, 0x00000001 }, //ar-TN -> ar 
                    { 0x00001C09, 0x00000009 }, //en-ZA -> en 
                    { 0x00001C0A, 0x0000000A }, //es-DO -> es 
                    { 0x00001C1A, 0x00006C1A }, //sr-Cyrl-BA -> sr-Cyrl
                    { 0x00001C3B, 0x0000783B }, //sma-SE -> sma 
                    { 0x00002001, 0x00000001 }, //ar-OM -> ar 
                    { 0x00002009, 0x00000009 }, //en-JM -> en 
                    { 0x0000200A, 0x0000000A }, //es-VE -> es 
                    { 0x0000201A, 0x0000641A }, //bs-Cyrl-BA -> bs-Cyrl
                    { 0x0000203B, 0x0000743B }, //sms-FI -> sms 
                    { 0x00002401, 0x00000001 }, //ar-YE -> ar 
                    { 0x00002409, 0x00000009 }, //en-029 -> en 
                    { 0x0000240A, 0x0000000A }, //es-CO -> es 
                    { 0x0000241A, 0x0000701A }, //sr-Latn-RS -> sr-Latn
                    { 0x0000243B, 0x0000703B }, //smn-FI -> smn 
                    { 0x00002801, 0x00000001 }, //ar-SY -> ar 
                    { 0x00002809, 0x00000009 }, //en-BZ -> en 
                    { 0x0000280A, 0x0000000A }, //es-PE -> es 
                    { 0x0000281A, 0x00006C1A }, //sr-Cyrl-RS -> sr-Cyrl
                    { 0x00002C01, 0x00000001 }, //ar-JO -> ar 
                    { 0x00002C09, 0x00000009 }, //en-TT -> en 
                    { 0x00002C0A, 0x0000000A }, //es-AR -> es 
                    { 0x00002C1A, 0x0000701A }, //sr-Latn-ME -> sr-Latn
                    { 0x00003001, 0x00000001 }, //ar-LB -> ar 
                    { 0x00003009, 0x00000009 }, //en-ZW -> en 
                    { 0x0000300A, 0x0000000A }, //es-EC -> es 
                    { 0x0000301A, 0x00006C1A }, //sr-Cyrl-ME -> sr-Cyrl
                    { 0x00003401, 0x00000001 }, //ar-KW -> ar 
                    { 0x00003409, 0x00000009 }, //en-PH -> en 
                    { 0x0000340A, 0x0000000A }, //es-CL -> es 
                    { 0x00003801, 0x00000001 }, //ar-AE -> ar 
                    { 0x0000380A, 0x0000000A }, //es-UY -> es 
                    { 0x00003C01, 0x00000001 }, //ar-BH -> ar 
                    { 0x00003C0A, 0x0000000A }, //es-PY -> es 
                    { 0x00004001, 0x00000001 }, //ar-QA -> ar 
                    { 0x00004009, 0x00000009 }, //en-IN -> en 
                    { 0x0000400A, 0x0000000A }, //es-BO -> es 
                    { 0x00004409, 0x00000009 }, //en-MY -> en 
                    { 0x0000440A, 0x0000000A }, //es-SV -> es 
                    { 0x00004809, 0x00000009 }, //en-SG -> en 
                    { 0x0000480A, 0x0000000A }, //es-HN -> es 
                    { 0x00004C0A, 0x0000000A }, //es-NI -> es 
                    { 0x0000500A, 0x0000000A }, //es-PR -> es 
                    { 0x0000540A, 0x0000000A }, //es-US -> es 
                    { 0x0000641A, 0x0000781A }, //bs-Cyrl -> bs
                    { 0x0000681A, 0x0000781A }, //bs-Latn -> bs
                    { 0x00006C1A, 0x00007C1A }, //sr-Cyrl -> sr
                    { 0x0000701A, 0x00007C1A }, //sr-Latn -> sr
                    { 0x0000742C, 0x0000002C }, //az-Cyrl -> az
                    { 0x0000782C, 0x0000002C }, //az-Latn -> az
                    { 0x00007843, 0x00000043 }, //uz-Cyrl -> uz
                    { 0x00007850, 0x00000050 }, //mn-Cyrl -> mn
                    { 0x0000785D, 0x0000005D }, //iu-Cans -> iu
                    { 0x00007C04, 0x00007804 }, //zh-Hant -> zh
                    { 0x00007C28, 0x00000028 }, //tg-Cyrl -> tg
                    { 0x00007C43, 0x00000043 }, //uz-Latn -> uz
                    { 0x00007C50, 0x00000050 }, //mn-Mong -> mn
                    { 0x00007C5D, 0x0000005D }, //iu-Latng -> iu
                    { 0x00007C5F, 0x0000005F }, //tzm-Latn -> tzm
                    { 0x00007C68, 0x00000068 }, //ha-Latn -> ha
                    { 0x0001007F, 0x0000007F }, //x-IV_mathan ->  ""
                    { 0x00010407, 0x00000007 }, //de-DE_phoneb -> de 
                    { 0x0001040E, 0x0000000E }, //hu-HU_technl -> hu 
                    { 0x00010437, 0x00000037 }, //ka-GE_modern -> ka 
                    { 0x00020804, 0x00000004 }, //zh-CN_stroke -> zh-Hans 
                    { 0x00021004, 0x00000004 }, //zh-SG_stroke -> zh-Hans 
                    { 0x00021404, 0x00007C04 }, //zh-MO_stroke -> zh-Hant 
                    { 0x00030404, 0x00007C04 }, //zh-TW_pronun -> zh-Hant 
                    { 0x00040404, 0x00007C04 }, //zh-TW_radstr -> zh-Hant 
                    { 0x00040411, 0x00000011 }, //ja-JP_radstr -> ja 
                    { 0x00040C04, 0x00007C04 }, //zh-HK_radstr -> zh-Hant 
                    { 0x00041404, 0x00007C04 }, //zh-MO_radstr -> zh-Hant 
                };


                int bottom = 0;
                int top = _countof(FallbackTable) - 1;

                while (bottom <= top)
                {
                    int middle = (bottom + top) / 2;
                    int testIndex = Locale - FallbackTable[middle].Locale;

                    if (testIndex == 0)
                    {
                        return FallbackTable[middle].Fallback;
                    }

                    if (testIndex < 0)
                        top = middle - 1;
                    else
                        bottom = middle + 1;
                }

                return 0;

            }

            //尝试将一个中性语言匹配到实际区域
            static LPCWSTR __fastcall DownlevelNeutralToSpecificLocaleName(LPCWSTR szLocaleName)
            {
                struct NeutralToSpecific
                {
                    LPCWSTR szNeutralLocale;
                    LPCWSTR szSpecificLocale;
                };


                static constexpr NeutralToSpecific NeutralToSpecificMap[] =
                {
                    { L"af", L"af-ZA" },
                    { L"am", L"am-ET" },
                    { L"ar", L"ar-SA" },
                    { L"arn", L"arn-CL" },
                    { L"as", L"as-IN" },
                    { L"az", L"az-Latn-AZ" },
                    { L"az-Cyrl", L"az-Cyrl-AZ" },
                    { L"az-Latn", L"az-Latn-AZ" },
                    { L"ba", L"ba-RU" },
                    { L"be", L"be-BY" },
                    { L"bg", L"bg-BG" },
                    { L"bn", L"bn-IN" },
                    { L"bo", L"bo-CN" },
                    { L"br", L"br-FR" },
                    { L"bs-Cyrl", L"bs-Cyrl-BA" },
                    { L"bs-Latn", L"bs-Latn-BA" },
                    { L"ca", L"ca-ES" },
                    { L"co", L"co-FR" },
                    { L"cs", L"cs-CZ" },
                    { L"cy", L"cy-GB" },
                    { L"da", L"da-DK" },
                    { L"de", L"de-DE" },
                    { L"dsb", L"dsb-DE" },
                    { L"dv", L"dv-MV" },
                    { L"el", L"el-GR" },
                    { L"en", L"en-US" },
                    { L"es", L"es-ES" },
                    { L"et", L"et-EE" },
                    { L"eu", L"eu-ES" },
                    { L"fa", L"fa-IR" },
                    { L"fi", L"fi-FI" },
                    { L"fil", L"fil-PH" },
                    { L"fo", L"fo-FO" },
                    { L"fr", L"fr-FR" },
                    { L"fy", L"fy-NL" },
                    { L"ga", L"ga-IE" },
                    { L"gd", L"gd-GB" },
                    { L"gl", L"gl-ES" },
                    { L"gsw", L"gsw-FR" },
                    { L"gu", L"gu-IN" },
                    { L"ha-Latn", L"ha-Latn-NG" },
                    { L"he", L"he-IL" },
                    { L"hi", L"hi-IN" },
                    { L"hr", L"hr-HR" },
                    { L"hsb", L"hsb-DE" },
                    { L"hu", L"hu-HU" },
                    { L"hy", L"hy-AM" },
                    { L"id", L"id-ID" },
                    { L"ig", L"ig-NG" },
                    { L"ii", L"ii-CN" },
                    { L"is", L"is-IS" },
                    { L"it", L"it-IT" },
                    { L"iu-Cans", L"iu-Cans-CA" },
                    { L"iu-Latn", L"iu-Latn-CA" },
                    { L"ja", L"ja-JP" },
                    { L"ka", L"ka-GE" },
                    { L"kk", L"kk-KZ" },
                    { L"kl", L"kl-GL" },
                    { L"km", L"km-KH" },
                    { L"kn", L"kn-IN" },
                    { L"ko", L"ko-KR" },
                    { L"kok", L"kok-IN" },
                    { L"ky", L"ky-KG" },
                    { L"lb", L"lb-LU" },
                    { L"lo", L"lo-LA" },
                    { L"lt", L"lt-LT" },
                    { L"lv", L"lv-LV" },
                    { L"mi", L"mi-NZ" },
                    { L"mk", L"mk-MK" },
                    { L"ml", L"ml-IN" },
                    { L"mn", L"mn-MN" },
                    { L"mn-Cyrl", L"mn-MN" },
                    { L"mn-Mong", L"mn-Mong-CN" },
                    { L"moh", L"moh-CA" },
                    { L"mr", L"mr-IN" },
                    { L"ms", L"ms-MY" },
                    { L"mt", L"mt-MT" },
                    { L"nb", L"nb-NO" },
                    { L"ne", L"ne-NP" },
                    { L"nl", L"nl-NL" },
                    { L"nn", L"nn-NO" },
                    { L"no", L"nb-NO" },
                    { L"nso", L"nso-ZA" },
                    { L"oc", L"oc-FR" },
                    { L"or", L"or-IN" },
                    { L"pa", L"pa-IN" },
                    { L"pl", L"pl-PL" },
                    { L"prs", L"prs-AF" },
                    { L"ps", L"ps-AF" },
                    { L"pt", L"pt-BR" },
                    { L"qut", L"qut-GT" },
                    { L"quz", L"quz-BO" },
                    { L"rm", L"rm-CH" },
                    { L"ro", L"ro-RO" },
                    { L"ru", L"ru-RU" },
                    { L"rw", L"rw-RW" },
                    { L"sa", L"sa-IN" },
                    { L"sah", L"sah-RU" },
                    { L"se", L"se-NO" },
                    { L"si", L"si-LK" },
                    { L"sk", L"sk-SK" },
                    { L"sl", L"sl-SI" },
                    { L"sma", L"sma-SE" },
                    { L"smj", L"smj-SE" },
                    { L"smn", L"smn-FI" },
                    { L"sms", L"sms-FI" },
                    { L"sq", L"sq-AL" },
                    { L"sr", L"sr-Latn-RS" },
                    { L"sr-Cyrl", L"sr-Cyrl-RS" },
                    { L"sr-Latn", L"sr-Latn-RS" },
                    { L"sv", L"sv-SE" },
                    { L"sw", L"sw-KE" },
                    { L"syr", L"syr-SY" },
                    { L"ta", L"ta-IN" },
                    { L"te", L"te-IN" },
                    { L"tg-Cyrl", L"tg-Cyrl-TJ" },
                    { L"th", L"th-TH" },
                    { L"tk", L"tk-TM" },
                    { L"tn", L"tn-ZA" },
                    { L"tr", L"tr-TR" },
                    { L"tt", L"tt-RU" },
                    { L"tzm-Latn", L"tzm-Latn-DZ" },
                    { L"ug", L"ug-CN" },
                    { L"uk", L"uk-UA" },
                    { L"ur", L"ur-PK" },
                    { L"uz", L"uz-Latn-UZ" },
                    { L"uz-Cyrl", L"uz-Cyrl-UZ" },
                    { L"uz-Latn", L"uz-Latn-UZ" },
                    { L"vi", L"vi-VN" },
                    { L"wo", L"wo-SN" },
                    { L"xh", L"xh-ZA" },
                    { L"yo", L"yo-NG" },
                    { L"zh", L"zh-CN" },
                    { L"zh-CHS", L"zh-CN" },
                    { L"zh-CHT", L"zh-HK" },
                    { L"zh-Hans", L"zh-CN" },
                    { L"zh-Hant", L"zh-HK" },
                    { L"zu", L"zu-ZA" },
                };


                int bottom = 0;
                int top = _countof(NeutralToSpecificMap) - 1;

                while (bottom <= top)
                {
                    int middle = (bottom + top) / 2;
                    int testIndex = StringCompareIgnoreCaseByAscii(szLocaleName, NeutralToSpecificMap[middle].szNeutralLocale, LOCALE_NAME_MAX_LENGTH);

                    if (testIndex == 0)
                        return NeutralToSpecificMap[middle].szSpecificLocale;

                    if (testIndex < 0)
                        top = middle - 1;
                    else
                        bottom = middle + 1;
                }

                //找不到就直接返回本身
                return szLocaleName;
            }

            static LPCWSTR __fastcall DownlevelLCIDToLocaleName(LCID Locale)
            {
                // Map of LCID to locale name.
                struct LcidToLocaleName
                {
                    LCID    Locale;
                    LPCWSTR localeName;
                };

                // 微软原版存在问题，数据没有更新。而且没有考虑排序。
                // Map of LCID to locale name for Windows XP.
                // Data in this table has been obtained from National Language Support (NLS) API Reference at
                // http://msdn.microsoft.com/en-us/goglobal/bb896001.aspx
                // The table is sorted to improve search performance.
                static constexpr LcidToLocaleName LcidToLocaleNameTable[] =
                {
                    { 0x00000001, L"ar" },
                    { 0x00000002, L"bg" },
                    { 0x00000003, L"ca" },
                    { 0x00000004, L"zh-Hans" },
                    { 0x00000005, L"cs" },
                    { 0x00000006, L"da" },
                    { 0x00000007, L"de" },
                    { 0x00000008, L"el" },
                    { 0x00000009, L"en" },
                    { 0x0000000A, L"es" },
                    { 0x0000000B, L"fi" },
                    { 0x0000000C, L"fr" },
                    { 0x0000000D, L"he" },
                    { 0x0000000E, L"hu" },
                    { 0x0000000F, L"is" },
                    { 0x00000010, L"it" },
                    { 0x00000011, L"ja" },
                    { 0x00000012, L"ko" },
                    { 0x00000013, L"nl" },
                    { 0x00000014, L"no" },
                    { 0x00000015, L"pl" },
                    { 0x00000016, L"pt" },
                    { 0x00000017, L"rm" },
                    { 0x00000018, L"ro" },
                    { 0x00000019, L"ru" },
                    { 0x0000001A, L"hr" },
                    { 0x0000001B, L"sk" },
                    { 0x0000001C, L"sq" },
                    { 0x0000001D, L"sv" },
                    { 0x0000001E, L"th" },
                    { 0x0000001F, L"tr" },
                    { 0x00000020, L"ur" },
                    { 0x00000021, L"id" },
                    { 0x00000022, L"uk" },
                    { 0x00000023, L"be" },
                    { 0x00000024, L"sl" },
                    { 0x00000025, L"et" },
                    { 0x00000026, L"lv" },
                    { 0x00000027, L"lt" },
                    { 0x00000029, L"fa" },
                    { 0x0000002A, L"vi" },
                    { 0x0000002B, L"hy" },
                    { 0x0000002C, L"az" },
                    { 0x0000002D, L"eu" },
                    { 0x0000002E, L"hsb" },
                    { 0x0000002F, L"mk" },
                    { 0x00000032, L"tn" },
                    { 0x00000034, L"xh" },
                    { 0x00000035, L"zu" },
                    { 0x00000036, L"af" },
                    { 0x00000037, L"ka" },
                    { 0x00000038, L"fo" },
                    { 0x00000039, L"hi" },
                    { 0x0000003A, L"mt" },
                    { 0x0000003B, L"se" },
                    { 0x0000003C, L"ga" },
                    { 0x0000003E, L"ms" },
                    { 0x0000003F, L"kk" },
                    { 0x00000040, L"ky" },
                    { 0x00000041, L"sw" },
                    { 0x00000042, L"tk" },
                    { 0x00000043, L"uz" },
                    { 0x00000044, L"tt" },
                    { 0x00000045, L"bn" },
                    { 0x00000046, L"pa" },
                    { 0x00000047, L"gu" },
                    { 0x00000048, L"or" },
                    { 0x00000049, L"ta" },
                    { 0x0000004A, L"te" },
                    { 0x0000004B, L"kn" },
                    { 0x0000004C, L"ml" },
                    { 0x0000004D, L"as" },
                    { 0x0000004E, L"mr" },
                    { 0x0000004F, L"sa" },
                    { 0x00000050, L"mn" },
                    { 0x00000051, L"bo" },
                    { 0x00000052, L"cy" },
                    { 0x00000053, L"km" },
                    { 0x00000054, L"lo" },
                    { 0x00000056, L"gl" },
                    { 0x00000057, L"kok" },
                    { 0x0000005A, L"syr" },
                    { 0x0000005B, L"si" },
                    { 0x0000005E, L"am" },
                    { 0x00000061, L"ne" },
                    { 0x00000062, L"fy" },
                    { 0x00000063, L"ps" },
                    { 0x00000064, L"fil" },
                    { 0x00000065, L"dv" },
                    { 0x0000006A, L"yo" },
                    { 0x0000006B, L"quz" },
                    { 0x0000006C, L"nso" },
                    { 0x0000006D, L"ba" },
                    { 0x0000006E, L"lb" },
                    { 0x0000006F, L"kl" },
                    { 0x00000070, L"ig" },
                    { 0x00000078, L"ii" },
                    { 0x0000007A, L"arn" },
                    { 0x0000007C, L"moh" },
                    { 0x0000007E, L"br" },
                    { 0x0000007F, L"" },
                    { 0x00000080, L"ug" },
                    { 0x00000081, L"mi" },
                    { 0x00000082, L"oc" },
                    { 0x00000083, L"co" },
                    { 0x00000084, L"gsw" },
                    { 0x00000085, L"sah" },
                    { 0x00000086, L"qut" },
                    { 0x00000087, L"rw" },
                    { 0x00000088, L"wo" },
                    { 0x0000008C, L"prs" },
                    { 0x00000091, L"gd" },
                    { 0x00000401, L"ar-SA" },
                    { 0x00000402, L"bg-BG" },
                    { 0x00000403, L"ca-ES" },
                    { 0x00000404, L"zh-TW" },
                    { 0x00000405, L"cs-CZ" },
                    { 0x00000406, L"da-DK" },
                    { 0x00000407, L"de-DE" },
                    { 0x00000408, L"el-GR" },
                    { 0x00000409, L"en-US" },
                    { 0x0000040A, L"es-ES_tradnl" },
                    { 0x0000040B, L"fi-FI" },
                    { 0x0000040C, L"fr-FR" },
                    { 0x0000040D, L"he-IL" },
                    { 0x0000040E, L"hu-HU" },
                    { 0x0000040F, L"is-IS" },
                    { 0x00000410, L"it-IT" },
                    { 0x00000411, L"ja-JP" },
                    { 0x00000412, L"ko-KR" },
                    { 0x00000413, L"nl-NL" },
                    { 0x00000414, L"nb-NO" },
                    { 0x00000415, L"pl-PL" },
                    { 0x00000416, L"pt-BR" },
                    { 0x00000417, L"rm-CH" },
                    { 0x00000418, L"ro-RO" },
                    { 0x00000419, L"ru-RU" },
                    { 0x0000041A, L"hr-HR" },
                    { 0x0000041B, L"sk-SK" },
                    { 0x0000041C, L"sq-AL" },
                    { 0x0000041D, L"sv-SE" },
                    { 0x0000041E, L"th-TH" },
                    { 0x0000041F, L"tr-TR" },
                    { 0x00000420, L"ur-PK" },
                    { 0x00000421, L"id-ID" },
                    { 0x00000422, L"uk-UA" },
                    { 0x00000423, L"be-BY" },
                    { 0x00000424, L"sl-SI" },
                    { 0x00000425, L"et-EE" },
                    { 0x00000426, L"lv-LV" },
                    { 0x00000427, L"lt-LT" },
                    { 0x00000428, L"tg-Cyrl-TJ" },
                    { 0x00000429, L"fa-IR" },
                    { 0x0000042A, L"vi-VN" },
                    { 0x0000042B, L"hy-AM" },
                    { 0x0000042C, L"az-Latn-AZ" },
                    { 0x0000042D, L"eu-ES" },
                    { 0x0000042E, L"hsb-DE" },
                    { 0x0000042F, L"mk-MK" },
                    { 0x00000432, L"tn-ZA" },
                    { 0x00000434, L"xh-ZA" },
                    { 0x00000435, L"zu-ZA" },
                    { 0x00000436, L"af-ZA" },
                    { 0x00000437, L"ka-GE" },
                    { 0x00000438, L"fo-FO" },
                    { 0x00000439, L"hi-IN" },
                    { 0x0000043A, L"mt-MT" },
                    { 0x0000043B, L"se-NO" },
                    { 0x0000043E, L"ms-MY" },
                    { 0x0000043F, L"kk-KZ" },
                    { 0x00000440, L"ky-KG" },
                    { 0x00000441, L"sw-KE" },
                    { 0x00000442, L"tk-TM" },
                    { 0x00000443, L"uz-Latn-UZ" },
                    { 0x00000444, L"tt-RU" },
                    { 0x00000445, L"bn-IN" },
                    { 0x00000446, L"pa-IN" },
                    { 0x00000447, L"gu-IN" },
                    { 0x00000448, L"or-IN" },
                    { 0x00000449, L"ta-IN" },
                    { 0x0000044A, L"te-IN" },
                    { 0x0000044B, L"kn-IN" },
                    { 0x0000044C, L"ml-IN" },
                    { 0x0000044D, L"as-IN" },
                    { 0x0000044E, L"mr-IN" },
                    { 0x0000044F, L"sa-IN" },
                    { 0x00000450, L"mn-MN" },
                    { 0x00000451, L"bo-CN" },
                    { 0x00000452, L"cy-GB" },
                    { 0x00000453, L"km-KH" },
                    { 0x00000454, L"lo-LA" },
                    { 0x00000456, L"gl-ES" },
                    { 0x00000457, L"kok-IN" },
                    { 0x0000045A, L"syr-SY" },
                    { 0x0000045B, L"si-LK" },
                    { 0x0000045D, L"iu-Cans-CA" },
                    { 0x0000045E, L"am-ET" },
                    { 0x00000461, L"ne-NP" },
                    { 0x00000462, L"fy-NL" },
                    { 0x00000463, L"ps-AF" },
                    { 0x00000464, L"fil-PH" },
                    { 0x00000465, L"dv-MV" },
                    { 0x00000468, L"ha-Latn-NG" },
                    { 0x0000046A, L"yo-NG" },
                    { 0x0000046B, L"quz-BO" },
                    { 0x0000046C, L"nso-ZA" },
                    { 0x0000046D, L"ba-RU" },
                    { 0x0000046E, L"lb-LU" },
                    { 0x0000046F, L"kl-GL" },
                    { 0x00000470, L"ig-NG" },
                    { 0x00000478, L"ii-CN" },
                    { 0x0000047A, L"arn-CL" },
                    { 0x0000047C, L"moh-CA" },
                    { 0x0000047E, L"br-FR" },
                    { 0x00000480, L"ug-CN" },
                    { 0x00000481, L"mi-NZ" },
                    { 0x00000482, L"oc-FR" },
                    { 0x00000483, L"co-FR" },
                    { 0x00000484, L"gsw-FR" },
                    { 0x00000485, L"sah-RU" },
                    { 0x00000486, L"qut-GT" },
                    { 0x00000487, L"rw-RW" },
                    { 0x00000488, L"wo-SN" },
                    { 0x0000048C, L"prs-AF" },
                    { 0x00000491, L"gd-GB" },
                    { 0x00000501, L"qps-ploc" },
                    { 0x000005FE, L"qps-ploca" },
                    { 0x00000801, L"ar-IQ" },
                    { 0x00000804, L"zh-CN" },
                    { 0x00000807, L"de-CH" },
                    { 0x00000809, L"en-GB" },
                    { 0x0000080A, L"es-MX" },
                    { 0x0000080C, L"fr-BE" },
                    { 0x00000810, L"it-CH" },
                    { 0x00000813, L"nl-BE" },
                    { 0x00000814, L"nn-NO" },
                    { 0x00000816, L"pt-PT" },
                    { 0x0000081A, L"sr-Latn-CS" },
                    { 0x0000081D, L"sv-FI" },
                    { 0x0000082C, L"az-Cyrl-AZ" },
                    { 0x0000082E, L"dsb-DE" },
                    { 0x0000083B, L"se-SE" },
                    { 0x0000083C, L"ga-IE" },
                    { 0x0000083E, L"ms-BN" },
                    { 0x00000843, L"uz-Cyrl-UZ" },
                    { 0x00000845, L"bn-BD" },
                    { 0x00000850, L"mn-Mong-CN" },
                    { 0x0000085D, L"iu-Latn-CA" },
                    { 0x0000085F, L"tzm-Latn-DZ" },
                    { 0x0000086B, L"quz-EC" },
                    { 0x000009FF, L"qps-plocm" },
                    { 0x00000C01, L"ar-EG" },
                    { 0x00000C04, L"zh-HK" },
                    { 0x00000C07, L"de-AT" },
                    { 0x00000C09, L"en-AU" },
                    { 0x00000C0A, L"es-ES" },
                    { 0x00000C0C, L"fr-CA" },
                    { 0x00000C1A, L"sr-Cyrl-CS" },
                    { 0x00000C3B, L"se-FI" },
                    { 0x00000C6B, L"quz-PE" },
                    { 0x00001001, L"ar-LY" },
                    { 0x00001004, L"zh-SG" },
                    { 0x00001007, L"de-LU" },
                    { 0x00001009, L"en-CA" },
                    { 0x0000100A, L"es-GT" },
                    { 0x0000100C, L"fr-CH" },
                    { 0x0000101A, L"hr-BA" },
                    { 0x0000103B, L"smj-NO" },
                    { 0x00001401, L"ar-DZ" },
                    { 0x00001404, L"zh-MO" },
                    { 0x00001407, L"de-LI" },
                    { 0x00001409, L"en-NZ" },
                    { 0x0000140A, L"es-CR" },
                    { 0x0000140C, L"fr-LU" },
                    { 0x0000141A, L"bs-Latn-BA" },
                    { 0x0000143B, L"smj-SE" },
                    { 0x00001801, L"ar-MA" },
                    { 0x00001809, L"en-IE" },
                    { 0x0000180A, L"es-PA" },
                    { 0x0000180C, L"fr-MC" },
                    { 0x0000181A, L"sr-Latn-BA" },
                    { 0x0000183B, L"sma-NO" },
                    { 0x00001C01, L"ar-TN" },
                    { 0x00001C09, L"en-ZA" },
                    { 0x00001C0A, L"es-DO" },
                    { 0x00001C1A, L"sr-Cyrl-BA" },
                    { 0x00001C3B, L"sma-SE" },
                    { 0x00002001, L"ar-OM" },
                    { 0x00002009, L"en-JM" },
                    { 0x0000200A, L"es-VE" },
                    { 0x0000201A, L"bs-Cyrl-BA" },
                    { 0x0000203B, L"sms-FI" },
                    { 0x00002401, L"ar-YE" },
                    { 0x00002409, L"en-029" },
                    { 0x0000240A, L"es-CO" },
                    { 0x0000241A, L"sr-Latn-RS" },
                    { 0x0000243B, L"smn-FI" },
                    { 0x00002801, L"ar-SY" },
                    { 0x00002809, L"en-BZ" },
                    { 0x0000280A, L"es-PE" },
                    { 0x0000281A, L"sr-Cyrl-RS" },
                    { 0x00002C01, L"ar-JO" },
                    { 0x00002C09, L"en-TT" },
                    { 0x00002C0A, L"es-AR" },
                    { 0x00002C1A, L"sr-Latn-ME" },
                    { 0x00003001, L"ar-LB" },
                    { 0x00003009, L"en-ZW" },
                    { 0x0000300A, L"es-EC" },
                    { 0x0000301A, L"sr-Cyrl-ME" },
                    { 0x00003401, L"ar-KW" },
                    { 0x00003409, L"en-PH" },
                    { 0x0000340A, L"es-CL" },
                    { 0x00003801, L"ar-AE" },
                    { 0x0000380A, L"es-UY" },
                    { 0x00003C01, L"ar-BH" },
                    { 0x00003C0A, L"es-PY" },
                    { 0x00004001, L"ar-QA" },
                    { 0x00004009, L"en-IN" },
                    { 0x0000400A, L"es-BO" },
                    { 0x00004409, L"en-MY" },
                    { 0x0000440A, L"es-SV" },
                    { 0x00004809, L"en-SG" },
                    { 0x0000480A, L"es-HN" },
                    { 0x00004C0A, L"es-NI" },
                    { 0x0000500A, L"es-PR" },
                    { 0x0000540A, L"es-US" },
                    { 0x0000641A, L"bs-Cyrl" },
                    { 0x0000681A, L"bs-Latn" },
                    { 0x00006C1A, L"sr-Cyrl" },
                    { 0x0000701A, L"sr-Latn" },
                    { 0x0000703B, L"smn" },
                    { 0x0000742C, L"az-Cyrl" },
                    { 0x0000743B, L"sms" },
                    { 0x00007804, L"zh" },
                    { 0x00007814, L"nn" },
                    { 0x0000782C, L"az-Latn" },
                    { 0x0000783B, L"sma" },
                    { 0x00007843, L"uz-Cyrl" },
                    { 0x00007850, L"mn-Cyrl" },
                    { 0x0000785D, L"iu-Cans" },
                    { 0x00007C04, L"zh-Hant" },
                    { 0x00007C14, L"nb" },
                    { 0x00007C1A, L"sr" },
                    { 0x00007C28, L"tg-Cyrl" },
                    { 0x00007C2E, L"dsb" },
                    { 0x00007C3B, L"smj" },
                    { 0x00007C43, L"uz-Latn" },
                    { 0x00007C50, L"mn-Mong" },
                    { 0x00007C5D, L"iu-Latn" },
                    { 0x00007C5F, L"tzm-Latn" },
                    { 0x00007C68, L"ha-Latn" },
                    { 0x0001007F, L"x-IV_mathan" },
                    { 0x00010407, L"de-DE_phoneb" },
                    { 0x0001040E, L"hu-HU_technl" },
                    { 0x00010437, L"ka-GE_modern" },
                    { 0x00020804, L"zh-CN_stroke" },
                    { 0x00021004, L"zh-SG_stroke" },
                    { 0x00021404, L"zh-MO_stroke" },
                    { 0x00030404, L"zh-TW_pronun" },
                    { 0x00040404, L"zh-TW_radstr" },
                    { 0x00040411, L"ja-JP_radstr" },
                    { 0x00040C04, L"zh-HK_radstr" },
                    { 0x00041404, L"zh-MO_radstr" },
                };

                int bottom = 0;
                int top = _countof(LcidToLocaleNameTable) - 1;

                while (bottom <= top)
                {
                    int middle = (bottom + top) / 2;
                    int testIndex = Locale - LcidToLocaleNameTable[middle].Locale;

                    if (testIndex == 0)
                    {
                        return LcidToLocaleNameTable[middle].localeName;
                    }

                    if (testIndex < 0)
                        top = middle - 1;
                    else
                        bottom = middle + 1;
                }


                return nullptr;
            }


            static bool __fastcall AddLangIDToBuffer(LANGID* LocaleBuffer, const ULONG BufferCount, ULONG& Count, const LANGID New)
            {
                //空间不足
                if (BufferCount <= Count)
                    return false;

                for (unsigned i = 0; i != Count; ++i)
                {
                    if (LocaleBuffer[i] == New)
                        return false;
                }

                LocaleBuffer[Count] = New;
                ++Count;
                return true;
            }
        }
    }
#endif
    
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    8,
    LCID,
    WINAPI,
    LocaleNameToLCID,
        _In_ LPCWSTR lpName,
        _In_ DWORD dwFlags
        )
    {
        if (auto pLocaleNameToLCID = try_get_LocaleNameToLCID())
        {
            return pLocaleNameToLCID(lpName, dwFlags);
        }

        //UNREFRENCED_PARAMETER(dwFlags);

        if (lpName == nullptr)
        {
            return GetUserDefaultLCID();
        }
        else if (StringCompareIgnoreCaseByAscii(lpName, LOCALE_NAME_SYSTEM_DEFAULT, _countof(LOCALE_NAME_SYSTEM_DEFAULT)) == 0)
        {
            return GetSystemDefaultLCID();
        }

        //Vista虽然不支持 LOCALE_ALLOW_NEUTRAL_NAMES，但是我们好人做到底，就直接支持了吧。
        if ((LOCALE_ALLOW_NEUTRAL_NAMES & dwFlags) == 0)
        {
            lpName = internal::DownlevelNeutralToSpecificLocaleName(lpName);
        }

        // Map of locale name to LCID.
        struct LocaleNameToLcid
        {
            const wchar_t*  localeName;
            LCID            lcid;
        };


        // Map of locale name to an index in LcidToLocaleNameTable, for Windows XP.
        // Data in this table has been obtained from National Language Support (NLS) API Reference at
        // http://msdn.microsoft.com/en-us/goglobal/bb896001.aspx
        // The table is sorted to improve search performance.
        static const LocaleNameToLcid LocaleNameToLcidTable[] =
        {
            { L"", 0x0000007F },
            { L"af", 0x00000036 },
            { L"af-ZA", 0x00000436 },
            { L"am", 0x0000005E },
            { L"am-ET", 0x0000045E },
            { L"ar", 0x00000001 },
            { L"ar-AE", 0x00003801 },
            { L"ar-BH", 0x00003C01 },
            { L"ar-DZ", 0x00001401 },
            { L"ar-EG", 0x00000C01 },
            { L"ar-IQ", 0x00000801 },
            { L"ar-JO", 0x00002C01 },
            { L"ar-KW", 0x00003401 },
            { L"ar-LB", 0x00003001 },
            { L"ar-LY", 0x00001001 },
            { L"ar-MA", 0x00001801 },
            { L"ar-OM", 0x00002001 },
            { L"ar-QA", 0x00004001 },
            { L"ar-SA", 0x00000401 },
            { L"ar-SY", 0x00002801 },
            { L"ar-TN", 0x00001C01 },
            { L"ar-YE", 0x00002401 },
            { L"arn", 0x0000007A },
            { L"arn-CL", 0x0000047A },
            { L"as", 0x0000004D },
            { L"as-IN", 0x0000044D },
            { L"az", 0x0000002C },
            { L"az-Cyrl", 0x0000742C },
            { L"az-Cyrl-AZ", 0x0000082C },
            { L"az-Latn", 0x0000782C },
            { L"az-Latn-AZ", 0x0000042C },
            { L"ba", 0x0000006D },
            { L"ba-RU", 0x0000046D },
            { L"be", 0x00000023 },
            { L"be-BY", 0x00000423 },
            { L"bg", 0x00000002 },
            { L"bg-BG", 0x00000402 },
            { L"bn", 0x00000045 },
            { L"bn-BD", 0x00000845 },
            { L"bn-IN", 0x00000445 },
            { L"bo", 0x00000051 },
            { L"bo-CN", 0x00000451 },
            { L"br", 0x0000007E },
            { L"br-FR", 0x0000047E },
            { L"bs-Cyrl", 0x0000641A },
            { L"bs-Cyrl-BA", 0x0000201A },
            { L"bs-Latn", 0x0000681A },
            { L"bs-Latn-BA", 0x0000141A },
            { L"ca", 0x00000003 },
            { L"ca-ES", 0x00000403 },
            { L"co", 0x00000083 },
            { L"co-FR", 0x00000483 },
            { L"cs", 0x00000005 },
            { L"cs-CZ", 0x00000405 },
            { L"cy", 0x00000052 },
            { L"cy-GB", 0x00000452 },
            { L"da", 0x00000006 },
            { L"da-DK", 0x00000406 },
            { L"de", 0x00000007 },
            { L"de-AT", 0x00000C07 },
            { L"de-CH", 0x00000807 },
            { L"de-DE", 0x00000407 },
            { L"de-DE_phoneb", 0x00010407 },
            { L"de-LI", 0x00001407 },
            { L"de-LU", 0x00001007 },
            { L"dsb", 0x00007C2E },
            { L"dsb-DE", 0x0000082E },
            { L"dv", 0x00000065 },
            { L"dv-MV", 0x00000465 },
            { L"el", 0x00000008 },
            { L"el-GR", 0x00000408 },
            { L"en", 0x00000009 },
            { L"en-029", 0x00002409 },
            { L"en-AU", 0x00000C09 },
            { L"en-BZ", 0x00002809 },
            { L"en-CA", 0x00001009 },
            { L"en-GB", 0x00000809 },
            { L"en-IE", 0x00001809 },
            { L"en-IN", 0x00004009 },
            { L"en-JM", 0x00002009 },
            { L"en-MY", 0x00004409 },
            { L"en-NZ", 0x00001409 },
            { L"en-PH", 0x00003409 },
            { L"en-SG", 0x00004809 },
            { L"en-TT", 0x00002C09 },
            { L"en-US", 0x00000409 },
            { L"en-ZA", 0x00001C09 },
            { L"en-ZW", 0x00003009 },
            { L"es", 0x0000000A },
            { L"es-AR", 0x00002C0A },
            { L"es-BO", 0x0000400A },
            { L"es-CL", 0x0000340A },
            { L"es-CO", 0x0000240A },
            { L"es-CR", 0x0000140A },
            { L"es-DO", 0x00001C0A },
            { L"es-EC", 0x0000300A },
            { L"es-ES", 0x00000C0A },
            { L"es-ES_tradnl", 0x0000040A },
            { L"es-GT", 0x0000100A },
            { L"es-HN", 0x0000480A },
            { L"es-MX", 0x0000080A },
            { L"es-NI", 0x00004C0A },
            { L"es-PA", 0x0000180A },
            { L"es-PE", 0x0000280A },
            { L"es-PR", 0x0000500A },
            { L"es-PY", 0x00003C0A },
            { L"es-SV", 0x0000440A },
            { L"es-US", 0x0000540A },
            { L"es-UY", 0x0000380A },
            { L"es-VE", 0x0000200A },
            { L"et", 0x00000025 },
            { L"et-EE", 0x00000425 },
            { L"eu", 0x0000002D },
            { L"eu-ES", 0x0000042D },
            { L"fa", 0x00000029 },
            { L"fa-IR", 0x00000429 },
            { L"fi", 0x0000000B },
            { L"fi-FI", 0x0000040B },
            { L"fil", 0x00000064 },
            { L"fil-PH", 0x00000464 },
            { L"fo", 0x00000038 },
            { L"fo-FO", 0x00000438 },
            { L"fr", 0x0000000C },
            { L"fr-BE", 0x0000080C },
            { L"fr-CA", 0x00000C0C },
            { L"fr-CH", 0x0000100C },
            { L"fr-FR", 0x0000040C },
            { L"fr-LU", 0x0000140C },
            { L"fr-MC", 0x0000180C },
            { L"fy", 0x00000062 },
            { L"fy-NL", 0x00000462 },
            { L"ga", 0x0000003C },
            { L"ga-IE", 0x0000083C },
            { L"gd", 0x00000091 },
            { L"gd-GB", 0x00000491 },
            { L"gl", 0x00000056 },
            { L"gl-ES", 0x00000456 },
            { L"gsw", 0x00000084 },
            { L"gsw-FR", 0x00000484 },
            { L"gu", 0x00000047 },
            { L"gu-IN", 0x00000447 },
            { L"ha-Latn", 0x00007C68 },
            { L"ha-Latn-NG", 0x00000468 },
            { L"he", 0x0000000D },
            { L"he-IL", 0x0000040D },
            { L"hi", 0x00000039 },
            { L"hi-IN", 0x00000439 },
            { L"hr", 0x0000001A },
            { L"hr-BA", 0x0000101A },
            { L"hr-HR", 0x0000041A },
            { L"hsb", 0x0000002E },
            { L"hsb-DE", 0x0000042E },
            { L"hu", 0x0000000E },
            { L"hu-HU", 0x0000040E },
            { L"hu-HU_technl", 0x0001040E },
            { L"hy", 0x0000002B },
            { L"hy-AM", 0x0000042B },
            { L"id", 0x00000021 },
            { L"id-ID", 0x00000421 },
            { L"ig", 0x00000070 },
            { L"ig-NG", 0x00000470 },
            { L"ii", 0x00000078 },
            { L"ii-CN", 0x00000478 },
            { L"is", 0x0000000F },
            { L"is-IS", 0x0000040F },
            { L"it", 0x00000010 },
            { L"it-CH", 0x00000810 },
            { L"it-IT", 0x00000410 },
            { L"iu-Cans", 0x0000785D },
            { L"iu-Cans-CA", 0x0000045D },
            { L"iu-Latn", 0x00007C5D },
            { L"iu-Latn-CA", 0x0000085D },
            { L"ja", 0x00000011 },
            { L"ja-JP", 0x00000411 },
            { L"ja-JP_radstr", 0x00040411 },
            { L"ka", 0x00000037 },
            { L"ka-GE", 0x00000437 },
            { L"ka-GE_modern", 0x00010437 },
            { L"kk", 0x0000003F },
            { L"kk-KZ", 0x0000043F },
            { L"kl", 0x0000006F },
            { L"kl-GL", 0x0000046F },
            { L"km", 0x00000053 },
            { L"km-KH", 0x00000453 },
            { L"kn", 0x0000004B },
            { L"kn-IN", 0x0000044B },
            { L"ko", 0x00000012 },
            { L"ko-KR", 0x00000412 },
            { L"kok", 0x00000057 },
            { L"kok-IN", 0x00000457 },
            { L"ky", 0x00000040 },
            { L"ky-KG", 0x00000440 },
            { L"lb", 0x0000006E },
            { L"lb-LU", 0x0000046E },
            { L"lo", 0x00000054 },
            { L"lo-LA", 0x00000454 },
            { L"lt", 0x00000027 },
            { L"lt-LT", 0x00000427 },
            { L"lv", 0x00000026 },
            { L"lv-LV", 0x00000426 },
            { L"mi", 0x00000081 },
            { L"mi-NZ", 0x00000481 },
            { L"mk", 0x0000002F },
            { L"mk-MK", 0x0000042F },
            { L"ml", 0x0000004C },
            { L"ml-IN", 0x0000044C },
            { L"mn", 0x00000050 },
            { L"mn-Cyrl", 0x00007850 },
            { L"mn-MN", 0x00000450 },
            { L"mn-Mong", 0x00007C50 },
            { L"mn-Mong-CN", 0x00000850 },
            { L"moh", 0x0000007C },
            { L"moh-CA", 0x0000047C },
            { L"mr", 0x0000004E },
            { L"mr-IN", 0x0000044E },
            { L"ms", 0x0000003E },
            { L"ms-BN", 0x0000083E },
            { L"ms-MY", 0x0000043E },
            { L"mt", 0x0000003A },
            { L"mt-MT", 0x0000043A },
            { L"nb", 0x00007C14 },
            { L"nb-NO", 0x00000414 },
            { L"ne", 0x00000061 },
            { L"ne-NP", 0x00000461 },
            { L"nl", 0x00000013 },
            { L"nl-BE", 0x00000813 },
            { L"nl-NL", 0x00000413 },
            { L"nn", 0x00007814 },
            { L"nn-NO", 0x00000814 },
            { L"no", 0x00000014 },
            { L"nso", 0x0000006C },
            { L"nso-ZA", 0x0000046C },
            { L"oc", 0x00000082 },
            { L"oc-FR", 0x00000482 },
            { L"or", 0x00000048 },
            { L"or-IN", 0x00000448 },
            { L"pa", 0x00000046 },
            { L"pa-IN", 0x00000446 },
            { L"pl", 0x00000015 },
            { L"pl-PL", 0x00000415 },
            { L"prs", 0x0000008C },
            { L"prs-AF", 0x0000048C },
            { L"ps", 0x00000063 },
            { L"ps-AF", 0x00000463 },
            { L"pt", 0x00000016 },
            { L"pt-BR", 0x00000416 },
            { L"pt-PT", 0x00000816 },
            { L"qps-ploc", 0x00000501 },
            { L"qps-ploca", 0x000005FE },
            { L"qps-plocm", 0x000009FF },
            { L"qut", 0x00000086 },
            { L"qut-GT", 0x00000486 },
            { L"quz", 0x0000006B },
            { L"quz-BO", 0x0000046B },
            { L"quz-EC", 0x0000086B },
            { L"quz-PE", 0x00000C6B },
            { L"rm", 0x00000017 },
            { L"rm-CH", 0x00000417 },
            { L"ro", 0x00000018 },
            { L"ro-RO", 0x00000418 },
            { L"ru", 0x00000019 },
            { L"ru-RU", 0x00000419 },
            { L"rw", 0x00000087 },
            { L"rw-RW", 0x00000487 },
            { L"sa", 0x0000004F },
            { L"sa-IN", 0x0000044F },
            { L"sah", 0x00000085 },
            { L"sah-RU", 0x00000485 },
            { L"se", 0x0000003B },
            { L"se-FI", 0x00000C3B },
            { L"se-NO", 0x0000043B },
            { L"se-SE", 0x0000083B },
            { L"si", 0x0000005B },
            { L"si-LK", 0x0000045B },
            { L"sk", 0x0000001B },
            { L"sk-SK", 0x0000041B },
            { L"sl", 0x00000024 },
            { L"sl-SI", 0x00000424 },
            { L"sma", 0x0000783B },
            { L"sma-NO", 0x0000183B },
            { L"sma-SE", 0x00001C3B },
            { L"smj", 0x00007C3B },
            { L"smj-NO", 0x0000103B },
            { L"smj-SE", 0x0000143B },
            { L"smn", 0x0000703B },
            { L"smn-FI", 0x0000243B },
            { L"sms", 0x0000743B },
            { L"sms-FI", 0x0000203B },
            { L"sq", 0x0000001C },
            { L"sq-AL", 0x0000041C },
            { L"sr", 0x00007C1A },
            { L"sr-Cyrl", 0x00006C1A },
            { L"sr-Cyrl-BA", 0x00001C1A },
            { L"sr-Cyrl-CS", 0x00000C1A },
            { L"sr-Cyrl-ME", 0x0000301A },
            { L"sr-Cyrl-RS", 0x0000281A },
            { L"sr-Latn", 0x0000701A },
            { L"sr-Latn-BA", 0x0000181A },
            { L"sr-Latn-CS", 0x0000081A },
            { L"sr-Latn-ME", 0x00002C1A },
            { L"sr-Latn-RS", 0x0000241A },
            { L"sv", 0x0000001D },
            { L"sv-FI", 0x0000081D },
            { L"sv-SE", 0x0000041D },
            { L"sw", 0x00000041 },
            { L"sw-KE", 0x00000441 },
            { L"syr", 0x0000005A },
            { L"syr-SY", 0x0000045A },
            { L"ta", 0x00000049 },
            { L"ta-IN", 0x00000449 },
            { L"te", 0x0000004A },
            { L"te-IN", 0x0000044A },
            { L"tg-Cyrl", 0x00007C28 },
            { L"tg-Cyrl-TJ", 0x00000428 },
            { L"th", 0x0000001E },
            { L"th-TH", 0x0000041E },
            { L"tk", 0x00000042 },
            { L"tk-TM", 0x00000442 },
            { L"tn", 0x00000032 },
            { L"tn-ZA", 0x00000432 },
            { L"tr", 0x0000001F },
            { L"tr-TR", 0x0000041F },
            { L"tt", 0x00000044 },
            { L"tt-RU", 0x00000444 },
            { L"tzm-Latn", 0x00007C5F },
            { L"tzm-Latn-DZ", 0x0000085F },
            { L"ug", 0x00000080 },
            { L"ug-CN", 0x00000480 },
            { L"uk", 0x00000022 },
            { L"uk-UA", 0x00000422 },
            { L"ur", 0x00000020 },
            { L"ur-PK", 0x00000420 },
            { L"uz", 0x00000043 },
            { L"uz-Cyrl", 0x00007843 },
            { L"uz-Cyrl-UZ", 0x00000843 },
            { L"uz-Latn", 0x00007C43 },
            { L"uz-Latn-UZ", 0x00000443 },
            { L"vi", 0x0000002A },
            { L"vi-VN", 0x0000042A },
            { L"wo", 0x00000088 },
            { L"wo-SN", 0x00000488 },
            { L"x-IV_mathan", 0x0001007F },
            { L"xh", 0x00000034 },
            { L"xh-ZA", 0x00000434 },
            { L"yo", 0x0000006A },
            { L"yo-NG", 0x0000046A },
            { L"zh", 0x00007804 },
            { L"zh-CHS", 0x00000004 },
            { L"zh-CHT", 0x00007C04 },
            { L"zh-CN", 0x00000804 },
            { L"zh-CN_stroke", 0x00020804 },
            { L"zh-Hans", 0x00000004 },
            { L"zh-Hant", 0x00007C04 },
            { L"zh-HK", 0x00000C04 },
            { L"zh-HK_radstr", 0x00040C04 },
            { L"zh-MO", 0x00001404 },
            { L"zh-MO_radstr", 0x00041404 },
            { L"zh-MO_stroke", 0x00021404 },
            { L"zh-SG", 0x00001004 },
            { L"zh-SG_stroke", 0x00021004 },
            { L"zh-TW", 0x00000404 },
            { L"zh-TW_pronun", 0x00030404 },
            { L"zh-TW_radstr", 0x00040404 },
            { L"zu", 0x00000035 },
            { L"zu-ZA", 0x00000435 },
        };



        int bottom = 0;
        int top = _countof(LocaleNameToLcidTable) - 1;

        while (bottom <= top)
        {
            int middle = (bottom + top) / 2;
            int testIndex = StringCompareIgnoreCaseByAscii(lpName, LocaleNameToLcidTable[middle].localeName, LOCALE_NAME_MAX_LENGTH);

            if (testIndex == 0)
                return LocaleNameToLcidTable[middle].lcid;

            if (testIndex < 0)
                top = middle - 1;
            else
                bottom = middle + 1;
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    int,
    WINAPI,
    LCIDToLocaleName,
        _In_ LCID     Locale,
        _Out_writes_opt_(cchName) LPWSTR  lpName,
        _In_ int      cchName,
        _In_ DWORD    dwFlags
        )
    {
        if (auto pLCIDToLocaleName = try_get_LCIDToLocaleName())
        {
            return pLCIDToLocaleName(Locale, lpName, cchName, dwFlags);
        }

        if (Locale == 0 || (lpName == nullptr && cchName > 0) || cchName < 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        auto szLocaleName = internal::DownlevelLCIDToLocaleName(Locale);

        if (szLocaleName)
        {
            //Vista虽然不支持 LOCALE_ALLOW_NEUTRAL_NAMES，但是我们好人做到底，就直接支持了吧。
            if ((LOCALE_ALLOW_NEUTRAL_NAMES & dwFlags) == 0)
            {
                szLocaleName = internal::DownlevelNeutralToSpecificLocaleName(szLocaleName);
            }

            auto count = wcslen(szLocaleName) + 1;

            if (cchName > 0)
            {
                if ((int)count > cchName)
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return 0;
                }

                memcpy(lpName, szLocaleName, count * sizeof(szLocaleName[0]));
            }

            return (int)count;
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    int,
    WINAPI,
    GetLocaleInfoEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ LCTYPE LCType,
        _Out_writes_to_opt_(cchData, return) LPWSTR lpLCData,
        _In_ int cchData
        )
    {
        if (auto pGetLocaleInfoEx = try_get_GetLocaleInfoEx())
        {
            return pGetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData);
        }

        auto Locale = LocaleNameToLCID(lpLocaleName, 0);

        if (Locale == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }
        return GetLocaleInfoW(Locale, LCType, lpLCData, cchData);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    24,
    int,
    WINAPI,
    GetNumberFormatEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ DWORD dwFlags,
        _In_ LPCWSTR lpValue,
        _In_opt_ CONST NUMBERFMTW *lpFormat,
        _Out_writes_opt_(cchNumber) LPWSTR lpNumberStr,
        _In_ int cchNumber
        )
    {
        if (auto pGetNumberFormatEx = try_get_GetNumberFormatEx())
        {
            return pGetNumberFormatEx(lpLocaleName, dwFlags, lpValue, lpFormat, lpNumberStr, cchNumber);
        }


        auto Locale = LocaleNameToLCID(lpLocaleName, 0);

        if (Locale == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        return GetNumberFormatW(Locale, dwFlags, lpValue, lpFormat, lpNumberStr, cchNumber);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    24,
    int,
    WINAPI,
    GetCurrencyFormatEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ DWORD dwFlags,
        _In_ LPCWSTR lpValue,
        _In_opt_ CONST CURRENCYFMTW *lpFormat,
        _Out_writes_opt_(cchCurrency) LPWSTR lpCurrencyStr,
        _In_ int cchCurrency
        )
    {
        if (auto pGetCurrencyFormatEx = try_get_GetCurrencyFormatEx())
        {
            return pGetCurrencyFormatEx(lpLocaleName, dwFlags, lpValue, lpFormat, lpCurrencyStr, cchCurrency);
        }


        auto Locale = LocaleNameToLCID(lpLocaleName, 0);

        if (Locale == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        return GetCurrencyFormatW(Locale, dwFlags, lpValue, lpFormat, lpCurrencyStr, cchCurrency);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    8,
    int,
    WINAPI,
    GetUserDefaultLocaleName,
        _Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
        _In_ int cchLocaleName
        )
    {
        if (auto pGetUserDefaultLocaleName = try_get_GetUserDefaultLocaleName())
        {
            return pGetUserDefaultLocaleName(lpLocaleName, cchLocaleName);
        }


        return LCIDToLocaleName(LOCALE_USER_DEFAULT, lpLocaleName, cchLocaleName, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    8,
    int,
    WINAPI,
    GetSystemDefaultLocaleName,
        _Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
        _In_ int cchLocaleName
        )
    {
        if (auto pGetSystemDefaultLocaleName = try_get_GetSystemDefaultLocaleName())
        {
            return pGetSystemDefaultLocaleName(lpLocaleName, cchLocaleName);
        }


        return LCIDToLocaleName(LOCALE_SYSTEM_DEFAULT, lpLocaleName, cchLocaleName, 0);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    24,
    BOOL,
    WINAPI,
    EnumCalendarInfoExEx,
        _In_ CALINFO_ENUMPROCEXEX pCalInfoEnumProcExEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ CALID Calendar,
        _In_opt_ LPCWSTR lpReserved,
        _In_ CALTYPE CalType,
        _In_ LPARAM lParam
        )
    {
        if (auto pEnumCalendarInfoExEx = try_get_EnumCalendarInfoExEx())
        {
            return pEnumCalendarInfoExEx(pCalInfoEnumProcExEx, lpLocaleName, Calendar, lpReserved, CalType, lParam);
        }

        LSTATUS lStatus;

        do
        {
            if (pCalInfoEnumProcExEx == nullptr)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            auto Locale = LocaleNameToLCID(lpLocaleName, 0);

            if (Locale == 0)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

#if defined(_X86_)
            constexpr const auto lParamOffset = 1;
            constexpr const auto pCallBackOffset = 16;

            static constexpr const byte ThunkData[] =
            {
                //lpCalendarInfoString = 0Ch + 4h
                //Calendar             = 08h + 8h
                0x68, 0x00, 0x00, 0x00, 0x00,       // push    lParam                              ; lParam
                0x6A, 0x00,                         // push    0                                   ; lpReserved
                0xFF, 0x74, 0x24, 0x10,             // push    dword ptr Calendar[esp]             ; Calendar
                0xFF, 0x74, 0x24, 0x10,             // push    dword ptr lpCalendarInfoString[esp] ; lpCalendarInfoString

                0xB8, 0x00, 0x00, 0x00, 0x00,       // mov     eax, pCalInfoEnumProcExEx
                0xFF, 0xD0,                         // call    eax
                0xC2, 0x08, 0x00,                   // retn    8
            };
#elif defined(_AMD64_)
            constexpr const auto lParamOffset = 2;
            constexpr const auto pCallBackOffset = 15;

            static constexpr const byte ThunkData[] =
            {
                //lpCalendarInfoString = rcx
                //Calendar             = edx
                0x49, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     r9, lParam
                0x45, 0x33, 0xC0,                                           // xor     r8d, r8d                 ; lpReserved

                0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rax, pCalInfoEnumProcExEx
                0xFF, 0xE0,                                                 // jmp     rax
            };
#endif

            auto _pThunkData = internal::HookThunkData::Alloc();
            if (!_pThunkData)
            {
                lStatus = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }
            static_assert(sizeof(_pThunkData->ShellCode) >= sizeof(ThunkData), "");
            memcpy(_pThunkData->ShellCode, ThunkData, sizeof(ThunkData));
            *(size_t*)(_pThunkData->ShellCode + lParamOffset) = lParam;
            *(size_t*)(_pThunkData->ShellCode + pCallBackOffset) = (size_t)pCalInfoEnumProcExEx;

            auto bSuccess = EnumCalendarInfoExW(
                CALINFO_ENUMPROCEXW(&_pThunkData->ShellCode),
                Locale,
                Calendar,
                CalType);

            lStatus = bSuccess ? ERROR_SUCCESS : GetLastError();
            _pThunkData->Free();
        } while (false);

        if (lStatus == ERROR_SUCCESS)
        {
            return TRUE;
        }
        else
        {
            SetLastError(lStatus);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows Vista,  Windows Server 2008
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    EnumDateFormatsExEx,
        _In_ DATEFMT_ENUMPROCEXEX lpDateFmtEnumProcExEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ DWORD dwFlags,
        _In_ LPARAM lParam
        )
    {
        if (auto pEnumDateFormatsExEx = try_get_EnumDateFormatsExEx())
        {
            return pEnumDateFormatsExEx(lpDateFmtEnumProcExEx, lpLocaleName, dwFlags, lParam);
        }

        LSTATUS lStatus;

        do
        {
            if (lpDateFmtEnumProcExEx == nullptr)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            auto Locale = LocaleNameToLCID(lpLocaleName, 0);

            if (Locale == 0)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

#if defined(_X86_)
            constexpr const auto lParamOffset = 1;
            constexpr const auto pCallBackOffset = 14;

            static constexpr const byte ThunkData[] =
            {
                //lpDateFormatString = 08h + 4h
                //CalendarID         = 04h + 8h
                0x68, 0x00, 0x00, 0x00, 0x00,       // push    lParam                              ; lParam
                0xFF, 0x74, 0x24, 0x0C,             // push    dword ptr CalendarID[esp]           ; CalendarID
                0xFF, 0x74, 0x24, 0x0C,             // push    dword ptr lpDateFormatString[esp]   ; lpDateFormatString

                0xB8, 0x00, 0x00, 0x00, 0x00,       // mov     eax, lpDateFmtEnumProcExEx
                0xFF, 0xD0,                         // call    eax
                0xC2, 0x08, 0x00,                   // retn    8
            };
#elif defined(_AMD64_)
            constexpr const auto lParamOffset = 2;
            constexpr const auto pCallBackOffset = 12;

            static constexpr const byte ThunkData[] =
            {
                //lpDateFormatString = rcx
                //CalendarID         = edx
                0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     r8, lParam

                0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rax, lpDateFmtEnumProcExEx
                0xFF, 0xE0,                                                 // jmp     rax
            };
#endif

            auto _pThunkData = internal::HookThunkData::Alloc();
            if (!_pThunkData)
            {
                lStatus = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }
            static_assert(sizeof(_pThunkData->ShellCode) >= sizeof(ThunkData), "");
            memcpy(_pThunkData->ShellCode, ThunkData, sizeof(ThunkData));
            *(size_t*)(_pThunkData->ShellCode + lParamOffset) = lParam;
            *(size_t*)(_pThunkData->ShellCode + pCallBackOffset) = (size_t)lpDateFmtEnumProcExEx;

            auto bSuccess = EnumDateFormatsExW(
                DATEFMT_ENUMPROCEXW(&_pThunkData->ShellCode),
                Locale,
                dwFlags);

            lStatus = bSuccess ? ERROR_SUCCESS : GetLastError();
            _pThunkData->Free();
        } while (false);

        if (lStatus == ERROR_SUCCESS)
        {
            return TRUE;
        }
        else
        {
            SetLastError(lStatus);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    36,
    int,
    WINAPI,
    LCMapStringEx,
        _In_opt_ LPCWSTR lpLocaleName,
        _In_ DWORD dwMapFlags,
        _In_reads_(cchSrc) LPCWSTR lpSrcStr,
        _In_ int cchSrc,
        _Out_writes_opt_(cchDest) LPWSTR lpDestStr,
        _In_ int cchDest,
        _In_opt_ LPNLSVERSIONINFO lpVersionInformation,
        _In_opt_ LPVOID lpReserved,
        _In_opt_ LPARAM sortHandle
        )
    {
        if (const auto pLCMapStringEx = try_get_LCMapStringEx())
        {
            return pLCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest, lpVersionInformation, lpReserved, sortHandle);
        }

        auto Locale = LocaleNameToLCID(lpLocaleName, 0);

        if (Locale == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        return LCMapStringW(Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    EnumSystemLocalesEx,
        _In_ LOCALE_ENUMPROCEX lpLocaleEnumProcEx,
        _In_ DWORD dwFlags,
        _In_ LPARAM lParam,
        _In_opt_ LPVOID lpReserved
        )
    {
        if (const auto pEnumSystemLocalesEx = try_get_EnumSystemLocalesEx())
        {
            return pEnumSystemLocalesEx(lpLocaleEnumProcEx, dwFlags, lParam, lpReserved);
        }
            
        LSTATUS lStatus = ERROR_SUCCESS;

        do
        {

            if (lpLocaleEnumProcEx == nullptr)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            if (dwFlags & ~(LOCALE_WINDOWS | LOCALE_SUPPLEMENTAL | LOCALE_ALTERNATE_SORTS))
            {
                lStatus = ERROR_INVALID_FLAGS;
                break;
            }

            DWORD dwLCID_Flags = 0;

            if (dwFlags == LOCALE_ALL)
            {
                dwFlags = LCID_SUPPORTED | LCID_ALTERNATE_SORTS;
            }
            else
            {
                dwFlags &= (LOCALE_WINDOWS | LOCALE_SUPPLEMENTAL | LOCALE_ALTERNATE_SORTS);
            }

#if defined(_X86_)
            constexpr auto lParamOffset = 1;
            constexpr auto pCallBackOffset = 12;

            static constexpr const byte ThunkData[] =
            {
                //lpLocaleString     = 08h + 4h
                0x68, 0x00, 0x00, 0x00, 0x00,       // push    lParam                              ; lParam
                0x6A, 0x00,                         // push    0                                   ; dwFlags(目前总是0)
                0xFF, 0x74, 0x24, 0x0C,             // push    dword ptr lpLocaleString[esp]       ; lpLocaleString

                0xB8, 0x00, 0x00, 0x00, 0x00,       // mov     eax, lpLocaleEnumProcEx
                0xFF, 0xD0,                         // call    eax
                0xC2, 0x04, 0x00,                   // retn    4
            };
#elif defined(_AMD64_)
            constexpr auto lParamOffset = 4;
            constexpr auto pCallBackOffset = 14;

            static constexpr const byte ThunkData[] =
            {
                //lpDateFormatString = rcx
                0x33, 0xD2,                                                 // xor     edx, edx                   ; dwFlags(0)
                0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     r8, lParam

                0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rax, lpDateFmtEnumProcExEx
                0xFF, 0xE0,                                                 // jmp     rax
            };
#endif

            auto _pThunkData = internal::HookThunkData::Alloc();
            if (!_pThunkData)
            {
                lStatus = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }
            static_assert(sizeof(_pThunkData->ShellCode) >= sizeof(ThunkData), "");
            memcpy(_pThunkData->ShellCode, ThunkData, sizeof(ThunkData));
            *(size_t*)(_pThunkData->ShellCode + lParamOffset) = lParam;
            *(size_t*)(_pThunkData->ShellCode + pCallBackOffset) = (size_t)lpLocaleEnumProcEx;

            auto bRet = EnumSystemLocalesW(LOCALE_ENUMPROCW(&_pThunkData->ShellCode), dwFlags);

            lStatus = bRet ? ERROR_SUCCESS : GetLastError();

            _pThunkData->Free();

        }while (false);

        if (lStatus == ERROR_SUCCESS)
        {
            return TRUE;
        }
        else
        {
            SetLastError(lStatus);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetThreadPreferredUILanguages,
        _In_ DWORD dwFlags,
        _Out_ PULONG pulNumLanguages,
        _Out_writes_opt_(*pcchLanguagesBuffer) PZZWSTR pwszLanguagesBuffer,
        _Inout_ PULONG pcchLanguagesBuffer
        )
    {
        if (auto const pGetThreadPreferredUILanguages = try_get_GetThreadPreferredUILanguages())
        {
            return pGetThreadPreferredUILanguages(dwFlags, pulNumLanguages, pwszLanguagesBuffer, pcchLanguagesBuffer);
        }

        LSTATUS lStatus = ERROR_SUCCESS;

        do
        {
            //参数验证
            if (pulNumLanguages == nullptr || pcchLanguagesBuffer == nullptr)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //cchLanguagesBuffer不为 0 时，pwszLanguagesBuffer 不能为 nullptr
            const ULONG cchLanguagesBuffer = *pcchLanguagesBuffer;
            if (cchLanguagesBuffer && pwszLanguagesBuffer == nullptr)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //让字符串正常截断
            if(pwszLanguagesBuffer)
                pwszLanguagesBuffer[0] = L'\0';
            if(cchLanguagesBuffer >= 2)
                pwszLanguagesBuffer[1] = L'\0';

            //LOCALE_CUSTOM_DEFAULT;
            //检测标记预期
            if (dwFlags & ~(MUI_LANGUAGE_ID | MUI_LANGUAGE_NAME | MUI_MERGE_SYSTEM_FALLBACK | MUI_MERGE_USER_FALLBACK | MUI_THREAD_LANGUAGES))
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }


            //MUI_LANGUAGE_ID 与 MUI_LANGUAGE_NAME不能同时使用
            if ((dwFlags & (MUI_LANGUAGE_ID | MUI_LANGUAGE_NAME)) == (MUI_LANGUAGE_ID | MUI_LANGUAGE_NAME))
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }


            //MUI_MERGE_USER_FALLBACK | MUI_MERGE_SYSTEM_FALLBACK 不能与 MUI_THREAD_LANGUAGES同时出现
            if ((dwFlags & (MUI_MERGE_USER_FALLBACK | MUI_MERGE_SYSTEM_FALLBACK)) && (dwFlags & MUI_THREAD_LANGUAGES))
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            //MUI_MERGE_USER_FALLBACK 是默认标记
            if ((dwFlags & (MUI_MERGE_SYSTEM_FALLBACK | MUI_MERGE_USER_FALLBACK | MUI_THREAD_LANGUAGES)) == 0)
            {
                dwFlags |= MUI_MERGE_USER_FALLBACK;
            }

                
            //最多可能为 3 个ID，线程上下文、User上下文、System上下文
            //WinXP不支持进程上下文……也不支持列表
            LANGID LanguageIds[3 * 5] = {};
            ULONG ulNumLanguages = 0;


            //检查线程上下文
            if ((dwFlags & (MUI_MERGE_USER_FALLBACK | MUI_THREAD_LANGUAGES)) )
            {
                auto LangID = (LANGID)GetThreadLocale();
                internal::AddLangIDToBuffer(LanguageIds, _countof(LanguageIds), ulNumLanguages, LangID);

                if (dwFlags & MUI_MERGE_SYSTEM_FALLBACK)
                {
                    for (; LangID = (LANGID)internal::DownlevelGetFallbackLocaleLCID(LangID);)
                    {
                        internal::AddLangIDToBuffer(LanguageIds, _countof(LanguageIds), ulNumLanguages, LangID);
                    }
                }
            }


            if (dwFlags & MUI_MERGE_USER_FALLBACK)
            {
                auto LangID = GetUserDefaultLangID();
                internal::AddLangIDToBuffer(LanguageIds, _countof(LanguageIds), ulNumLanguages, LangID);

                    
                for (; LangID = (LANGID)internal::DownlevelGetFallbackLocaleLCID(LangID);)
                {
                    internal::AddLangIDToBuffer(LanguageIds, _countof(LanguageIds), ulNumLanguages, LangID);
                }
            }

            if (dwFlags & MUI_MERGE_SYSTEM_FALLBACK)
            {
                auto LangID = GetSystemDefaultLangID();
                internal::AddLangIDToBuffer(LanguageIds, _countof(LanguageIds), ulNumLanguages, LangID);

                    
                for (; LangID = (LANGID)internal::DownlevelGetFallbackLocaleLCID(LangID);)
                {
                    internal::AddLangIDToBuffer(LanguageIds, _countof(LanguageIds), ulNumLanguages, LangID);
                }
            }
                


            if (dwFlags & MUI_LANGUAGE_ID)
            {
                //0804

                const ULONG cchLanguagesBufferNeed = _countof(L"0804") * ulNumLanguages + 1;

                *pcchLanguagesBuffer = cchLanguagesBufferNeed;

                if (pwszLanguagesBuffer)
                {
                    if (cchLanguagesBuffer < cchLanguagesBufferNeed)
                    {
                        lStatus = ERROR_INSUFFICIENT_BUFFER;
                        break;
                    }

                    constexpr const char Hex[] = "0123456789abcdef";


                    for (unsigned i = 0; i != ulNumLanguages; ++i)
                    {
                        const auto LanguageId = LanguageIds[i];

                        *pwszLanguagesBuffer++ = Hex[(LanguageId & 0xF000ul) >> 12];
                        *pwszLanguagesBuffer++ = Hex[(LanguageId & 0x0F00ul) >> 8];
                        *pwszLanguagesBuffer++ = Hex[(LanguageId & 0x00F0ul) >> 4];
                        *pwszLanguagesBuffer++ = Hex[(LanguageId & 0x000Ful) >> 0];
                        *pwszLanguagesBuffer++ = L'\0';
                    }

                    *pwszLanguagesBuffer = L'\0';
                }
                    
            }
            else
            {
                //zh-cn 风格
                ULONG cchLanguagesBufferNeed = 0;

                if (pwszLanguagesBuffer)
                {
                    wchar_t szLanguageNameStaticBuffer[LOCALE_NAME_MAX_LENGTH * _countof(LanguageIds) + 1];

                    //如果目标缓冲区足够容纳，那么我们直接使用目标缓冲区，否则我们先使用 szLanguageNameStaticBuffer 中转。
                    wchar_t* pBuffer = LOCALE_NAME_MAX_LENGTH * ulNumLanguages + 1 <= cchLanguagesBuffer ? pwszLanguagesBuffer : szLanguageNameStaticBuffer;
                    auto pTmp = pBuffer;

                    for (unsigned i = 0; i != ulNumLanguages; ++i)
                    {
                        const auto LanguageId = LanguageIds[i];

                        auto szLocaleName = internal::DownlevelLCIDToLocaleName(LanguageId);

                        if (szLocaleName)
                        {
                            auto result = wcslen(szLocaleName) + 1;

                            if (result <= LOCALE_NAME_MAX_LENGTH)
                            {
                                memcpy(pTmp, szLocaleName, result * sizeof(pTmp[0]));
                                pTmp += result;
                            }
                        }
                    }

                    *pTmp++ = L'\0';

                    cchLanguagesBufferNeed = static_cast<ULONG>(pTmp - pBuffer);

                    *pcchLanguagesBuffer = cchLanguagesBufferNeed;

                    if (pBuffer == szLanguageNameStaticBuffer)
                    {
                        if (cchLanguagesBuffer < cchLanguagesBufferNeed)
                        {
                            lStatus = ERROR_INSUFFICIENT_BUFFER;
                            break;
                        }

                        memcpy(pwszLanguagesBuffer, szLanguageNameStaticBuffer, cchLanguagesBufferNeed * sizeof(pwszLanguagesBuffer[0]));
                    }
                }
                else				
                {
                    for (unsigned i = 0; i != ulNumLanguages; ++i)
                    {
                        const auto LanguageId = LanguageIds[i];

                        auto szLocaleName = internal::DownlevelLCIDToLocaleName(LanguageId);

                        if (szLocaleName)
                        {
                            auto result = wcslen(szLocaleName) + 1;

                            if (result <= LOCALE_NAME_MAX_LENGTH)
                            {
                                cchLanguagesBufferNeed += static_cast<ULONG>(result);
                            }
                        }
                    }

                    ++cchLanguagesBufferNeed;

                    *pcchLanguagesBuffer = cchLanguagesBufferNeed;
                }
            }

            //返回语言数量
            *pulNumLanguages = ulNumLanguages;

            return TRUE;

        } while (false);


        SetLastError(lStatus);

        return FALSE;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps only]
    //Minimum supported server	Windows Server 2008 [desktop apps only]
    __DEFINE_THUNK(
    kernel32,
    0,
    LANGID,
    WINAPI,
    GetThreadUILanguage,
        void
        )
    {
        if (auto const _pfnGetThreadUILanguage = try_get_GetThreadUILanguage())
        {
            return _pfnGetThreadUILanguage();
        }

        return (LANGID)GetThreadLocale();
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

    //Minimum supported client	Windows 7 [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 R2 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    12,
    int,
    WINAPI,
    ResolveLocaleName,
        _In_opt_                        LPCWSTR lpNameToResolve,
        _Out_writes_opt_(cchLocaleName) LPWSTR  lpLocaleName,
        _In_                            int     cchLocaleName
        )
    {
        if (auto const pResolveLocaleName = try_get_ResolveLocaleName())
        {
            return pResolveLocaleName(lpNameToResolve, lpLocaleName, cchLocaleName);
        }

        LCID lcid = 0;

        if (lpNameToResolve == nullptr)
        {
            lcid = GetUserDefaultLCID();
        }
        else
        {
            wchar_t Buffer[LOCALE_NAME_MAX_LENGTH];

            unsigned i = 0;
            for (; i != _countof(Buffer) && lpNameToResolve[i]; ++i)
                Buffer[i] = lpNameToResolve[i];

            if (i == _countof(Buffer))
            {
                //输入异常
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;
            }

            //保证 '\0' 截断
            Buffer[i] = L'\0';

            for (;;)
            {
                lcid = LocaleNameToLCID(Buffer, 0);

                //成功找到了区域，那么停止搜索
                if (lcid != 0 && lcid != LOCALE_CUSTOM_UNSPECIFIED)
                    break;

                while(i)
                {
                    --i;

                    if (Buffer[i] == L'-')
                    {
                        Buffer[i] = L'\0';
                        break;
                    }
                }

                //字符串已经达到开始位置，则停止搜索
                if (i == 0)
                    break;
            }
        }

        if (lcid !=0 && lcid != LOCALE_CUSTOM_UNSPECIFIED)
        {
            if (lpLocaleName == nullptr || cchLocaleName == 0)
            {
                lpLocaleName = nullptr;
                cchLocaleName = 0;
            }

            //删除排序规则，然后重新转换名称。
            auto result = LCIDToLocaleName(LANGIDFROMLCID(lcid), lpLocaleName, cchLocaleName, 0);

            if (result)
                return result;
        }
            
        //如果找不到，那么直接设置为空字符串
        if (lpLocaleName && cchLocaleName)
        {
            lpLocaleName[0] = L'\0';
        }

        return 1;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    GetUserPreferredUILanguages,
        _In_ DWORD dwFlags,
        _Out_ PULONG pulNumLanguages,
        _Out_writes_opt_(*pcchLanguagesBuffer) PZZWSTR pwszLanguagesBuffer,
        _Inout_ PULONG pcchLanguagesBuffer
        )
    {
        if (auto const _pfnGetUserPreferredUILanguages = try_get_GetUserPreferredUILanguages())
        {
            return _pfnGetUserPreferredUILanguages(dwFlags, pulNumLanguages, pwszLanguagesBuffer, pcchLanguagesBuffer);
        }
            
        dwFlags &= ~(MUI_LANGUAGE_ID | MUI_LANGUAGE_NAME);
        dwFlags |= MUI_MERGE_USER_FALLBACK;
        return ::GetThreadPreferredUILanguages(dwFlags, pulNumLanguages, pwszLanguagesBuffer, pcchLanguagesBuffer);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    16,
    BOOL,
    WINAPI,
    EnumTimeFormatsEx,
        _In_ TIMEFMT_ENUMPROCEX _pfnTimeFmtEnumProcEx,
        _In_opt_ LPCWSTR _szLocaleName,
        _In_ DWORD _fFlags,
        _In_ LPARAM _lParam
        )
    {
        if (auto const _pfnEnumTimeFormatsEx = try_get_EnumTimeFormatsEx())
        {
            return _pfnEnumTimeFormatsEx(_pfnTimeFmtEnumProcEx, _szLocaleName, _fFlags, _lParam);
        }
        LSTATUS lStatus;

        do
        {
            if (_pfnTimeFmtEnumProcEx == nullptr)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

            auto _Locale = LocaleNameToLCID(_szLocaleName, 0);

            if (_Locale == 0)
            {
                lStatus = ERROR_INVALID_PARAMETER;
                break;
            }

#if defined(_X86_)
            constexpr const auto lParamOffset = 1;
            constexpr const auto pCallBackOffset = 10;

            static constexpr const byte ThunkData[] =
            {
                // _szTimeFormatString = 08h + 4h
                0x68, 0x00, 0x00, 0x00, 0x00,       // push    lParam                              ; lParam
                0xFF, 0x74, 0x24, 0x0C,             // push    dword ptr _szTimeFormatString[esp]  ; _szTimeFormatString

                0xB8, 0x00, 0x00, 0x00, 0x00,       // mov     eax, pfnTimeFmtEnumProcEx
                0xFF, 0xD0,                         // call    eax
                0xC2, 0x08, 0x00,                   // retn    4
            };
#elif defined(_AMD64_)
            constexpr const auto lParamOffset = 2;
            constexpr const auto pCallBackOffset = 12;

            static constexpr const byte ThunkData[] =
            {
                // _szTimeFormatString = rcx
                0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rdx, lParam

                0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov     rax, pfnTimeFmtEnumProcEx
                0xFF, 0xE0,                                                 // jmp     rax
            };
#endif

            auto _pThunkData = internal::HookThunkData::Alloc();
            if (!_pThunkData)
            {
                lStatus = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }

            static_assert(sizeof(_pThunkData->ShellCode) >= sizeof(ThunkData), "");
            memcpy(_pThunkData->ShellCode, ThunkData, sizeof(ThunkData));
            *(size_t*)(_pThunkData->ShellCode + lParamOffset) = _lParam;
            *(size_t*)(_pThunkData->ShellCode + pCallBackOffset) = (size_t)_pfnTimeFmtEnumProcEx;

            auto bSuccess = EnumTimeFormatsW(
                TIMEFMT_ENUMPROCW(&_pThunkData->ShellCode),
                _Locale,
                _fFlags);

            lStatus = bSuccess ? ERROR_SUCCESS : GetLastError();
            _pThunkData->Free();
        } while (false);

        if (lStatus == ERROR_SUCCESS)
        {
            return TRUE;
        }
        else
        {
            SetLastError(lStatus);
            return FALSE;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    28,
    int,
    WINAPI,
    GetCalendarInfoEx,
        _In_opt_ LPCWSTR _szLocaleName,
        _In_ CALID _Calendar,
        _In_opt_ LPCWSTR _szReserved,
        _In_ CALTYPE _CalType,
        _Out_writes_opt_(_cchData) LPWSTR _szCalData,
        _In_ int _cchData,
        _Out_opt_ LPDWORD _puValue
        )
    {
        if (auto const _pfnGetCalendarInfoEx = try_get_GetCalendarInfoEx())
        {
            return _pfnGetCalendarInfoEx(_szLocaleName, _Calendar, _szReserved, _CalType, _szCalData, _cchData, _puValue);
        }

        const auto _Locale = LocaleNameToLCID(_szLocaleName, 0);

        if (_Locale == 0)
        {
            return 0;
        }

        return GetCalendarInfoW(_Locale, _Calendar, _CalType, _szCalData, _cchData, _puValue);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    12,
    BOOL,
    WINAPI,
    GetNLSVersionEx,
        _In_	    NLS_FUNCTION _uFunction,
        _In_opt_    LPCWSTR _szLocaleName,
        _Inout_	    LPNLSVERSIONINFOEX _pVersionInformation
        )
    {
        if (auto const _pfnGetNLSVersionEx = try_get_GetNLSVersionEx())
        {
            return _pfnGetNLSVersionEx(_uFunction, _szLocaleName, _pVersionInformation);
        }
            
        if (_uFunction != COMPARE_STRING)
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return FALSE;
        }

        if (_pVersionInformation->dwNLSVersionInfoSize >= 12/*sizeof(NLSVERSIONINFO)*/)
        {
            memset(&_pVersionInformation->dwNLSVersionInfoSize + 1, 0, _pVersionInformation->dwNLSVersionInfoSize - sizeof(_pVersionInformation->dwNLSVersionInfoSize));
            return TRUE;
        }
        else
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }     
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista [desktop apps | UWP apps]
    //Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    kernel32,
    20,
    BOOL,
    WINAPI,
    IsNLSDefinedString,
        _In_  NLS_FUNCTION _uFunction,
        _In_ DWORD _fFlags,
        _In_ LPNLSVERSIONINFO _pVersionInformation,
        _In_reads_(_cchStr) LPCWSTR _szString,
        _In_ INT _cchStr
        )
    {
        if (auto const _pfnIsNLSDefinedString = try_get_IsNLSDefinedString())
        {
            return _pfnIsNLSDefinedString(_uFunction, _fFlags, _pVersionInformation, _szString, _cchStr);
        }

        if (_uFunction != COMPARE_STRING)
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return FALSE;
        }

        if (_pVersionInformation->dwNLSVersionInfoSize < 12/*sizeof(NLSVERSIONINFO)*/)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        if (_cchStr < 0)
            _cchStr = (int)wcslen(_szString);


        for (; _cchStr;--_cchStr, ++_szString)
        {
            UINT16 _ch = *_szString;

            if (_ch >= 0xe000u && _ch <= 0xf8ffu)
            {
                return FALSE;
            }

            if (IS_LOW_SURROGATE(_ch))
                return FALSE;

            if (IS_HIGH_SURROGATE(_ch))
            {
                --_cchStr;
                ++_szString;
                if (_cchStr == 0)
                    return FALSE;
                _ch = *_szString;

                if (!IS_LOW_SURROGATE(_ch))
                    return FALSE;
                continue;
            }

            WORD _CharType;
            if (GetStringTypeW(CT_CTYPE1, _szString, 1, &_CharType) && (_CharType & C1_DEFINED) == 0)
            {
                return FALSE;
            }
        }

        return TRUE;
            
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    kernel32,
    40,
    int,
    WINAPI,
    FindNLSStringEx,
        _In_opt_ LPCWSTR _szLocaleName,
        _In_ DWORD _fFindNLSStringFlags,
        _In_reads_(_cchSource) LPCWSTR _szStringSource,
        _In_ int _cchSource,
        _In_reads_(_cchValue) LPCWSTR _szStringValue,
        _In_ int _cchValue,
        _Out_opt_ LPINT _pcchFound,
        _In_opt_ LPNLSVERSIONINFO _pVersionInformation,
        _In_opt_ LPVOID _pReserved,
        _In_opt_ LPARAM _hSortHandle
        )
    {
        if (auto const _pfnFindNLSStringEx = try_get_FindNLSStringEx())
        {
            return _pfnFindNLSStringEx(_szLocaleName, _fFindNLSStringFlags, _szStringSource, _cchSource, _szStringValue, _cchValue, _pcchFound, _pVersionInformation, _pReserved, _hSortHandle);
        }

        __WarningMessage__("FindNLSStringEx 暂时只支持搜索 _cchValue 的子字符串。");

        const DWORD _fFindFlags = _fFindNLSStringFlags & (FIND_STARTSWITH | FIND_ENDSWITH | FIND_FROMSTART | FIND_FROMEND);
        if (_fFindFlags & (_fFindFlags - 1))
        {
            SetLastError(ERROR_INVALID_FLAGS);
            return -1;
        }

        if (_pVersionInformation || _pReserved|| _hSortHandle
            || _szStringSource == nullptr || _cchSource == 0 || _cchSource < -1
            || _szStringValue == nullptr || _cchValue == 0 || _cchValue < -1)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return -1;
        }

        const auto _Locale = LocaleNameToLCID(_szLocaleName, 0);
        if (_Locale == 0)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return -1;
        }

        if (_cchSource == -1)
        {
            _cchSource = wcslen(_szStringSource);
        }

        if (_cchValue == -1)
        {
            _cchValue = wcslen(_szStringValue);
        }

        if (_cchSource < _cchValue)
            return -1;

        const DWORD _fCmpFlags = _fFindNLSStringFlags & ~(FIND_STARTSWITH | FIND_ENDSWITH | FIND_FROMSTART | FIND_FROMEND);
        if ((_fFindNLSStringFlags & (FIND_ENDSWITH | FIND_FROMEND)) == 0)
        {
            // 从头开始搜索
            if (_fFindNLSStringFlags & FIND_STARTSWITH)
            {
                const auto _nResult = CompareStringW(_Locale, _fCmpFlags, _szStringSource, _cchValue, _szStringValue, _cchValue);
                if (_nResult == CSTR_EQUAL)
                {
                    if (_pcchFound)
                        *_pcchFound = _cchValue;

                    return 0;
                }
            }
            else
            {
                auto _szStr = _szStringSource;
                auto _szStrEnd = _szStringSource + _cchSource - _cchValue + 1;
                for (; _szStr != _szStrEnd;++_szStr)
                {
                    const auto _nResult = CompareStringW(_Locale, _fCmpFlags, _szStr, _cchValue, _szStringValue, _cchValue);
                    if (_nResult == 0)
                        return -1;

                    if (_nResult == CSTR_EQUAL)
                    {
                        if (_pcchFound)
                            *_pcchFound = _cchValue;

                        return _szStr - _szStringSource;
                    }
                }
            }
        }
        else
        {
            // 反向搜索
            auto _szStr = _szStringSource + _cchSource - _cchValue;
            if (_fFindNLSStringFlags & FIND_ENDSWITH)
            {
                const auto _nResult = CompareStringW(_Locale, _fCmpFlags, _szStr, _cchValue, _szStringValue, _cchValue);
                if (_nResult == CSTR_EQUAL)
                {
                    if (_pcchFound)
                        *_pcchFound = _cchValue;

                    return _szStr - _szStringSource;
                }
            }
            else
            {
                for(;; --_szStr)
                {
                    const auto _nResult = CompareStringW(_Locale, _fCmpFlags, _szStr, _cchValue, _szStringValue, _cchValue);
                    if (_nResult == 0)
                        return -1;

                    if (_nResult == CSTR_EQUAL)
                    {
                        if (_pcchFound)
                            *_pcchFound = _cchValue;

                        return _szStr - _szStringSource;
                    }
                    
                    if (_szStr == _szStringSource)
                        break;
                } 
            }
        }

        return -1;
    }
#endif
} //namespace YY
