Obecné:

- každá knihovna musí exportovat alespoň jednu do_get_*_descriptors metodu (do_get_filter_descriptors, model, signal, metric, solver, approximator)
- pokud knihovna exportuje do_get_*_descriptors, musí exportovat i nějakou další create/solve metodu podle tohoto klíče:
	- do_get_filter_descriptors --> do_create_filter
	- do_get_signal_descriptors --> do_create_signal
	- do_get_metric_descriptors --> do_create_metric
	- do_get_solver_descriptors --> do_solve_generic
	- do_get_approximator_descriptors --> do_create_approximator
- filtr, signál, aproximátor i metrika by měly jít příslušnou funkcí vytvořit
- žádná knihovna nesmí vytvořit entitu, kterou neobsahuje v deskriptoru (pro případ kdy např. zapomeneme else blok nebo tak něco - testovat se to dá s náhodným nebo neplatným GUID na vstupu)
- do_get_*_descriptors, do_create_* a do_solve.. funkce obecně validují všechny vstupy (že nejsou nullptr, u deskriptorů že begin != end, apod.)

Filtry obecné:

- Information, Warning a Error událost nesmí být nikdy maskována (projde v nezměněné podobě)
- Shut_Down událost ukončí činnost filtru a nesmí být nikdy maskována (projde dál; filtr pak už nepřijímá další eventy - Execute vrací chybový kód)
- Warm_Reset nikdy není maskován
- z deskriptorů by u některých parametrů mělo být zřejmé, jak testovat

Modely obecně:

- každý model má buď příznak scgms::NModel_Flags::Discrete_Model nebo Signal_Model
- pokud má model příznak (v deskriptoru) Discrete_Model, měl by jít vytvořit funkcí do_create_discrete_model
- analogicky pokud tento příznak nemá, nesmí jít vytvořit
- diskrétní modely:
	- implementují rozhraní filtru, tzn. musí taktéž splňovat to co je psáno výše (Filtry obecné)
	- nesmí jít vytvořit, pokud je na vstupu do_create_discrete_model vektor parametrů jinak velký, než je v deskriptoru (number_of_parameters)
	- jejich metoda Initialize musí jít volat právě jednou na začátku (opakované volání vrací chybu)
	- metoda Step musí jít volat s parametrem time_advance_delta == 0.0 (pak by měl model posílat dál současný stav) nebo větší (pak model může a nemusí něco poslat)

Metriky obecně:

- musí vykazovat vlastnosti metriky ( https://en.wikipedia.org/wiki/Metric_(mathematics) )
	- identita (d(a,a) = 0, pokud je chybová a referenční hodnota v jeden čas stejná, metrika je 0)
	- symetrie (d(a,b) = d(b,a), metrika mezi chybovou a referenční hodnotou je v jeden čas stejná, i když je prohodím)
	- trojúhelníková nerovnost (d(a,x) <= d(a,y) + d(x,y))

Aproximátory obecně:

- když dodáte signál s třeba 20 hodnotami, musí to umět aproximovat určitě prostředek (když si řeknete o číslo v čase mezi 10. a 11.)
- stejně tak to musí umět poskytnout derivaci v tom bodě


============


Filtry:

data - pro teď vynecháme

drawing filtr
- konfigurace:
	- cesty musí být platné
	- cesty musí být navzájem různé
	- velikosti obrázku musí být kladné nenulové (jinak musí konfigurace selhat)
- funkce:
	- po spuštění a execute alespoň jednoho eventu se musí vytvořit obrázky na zadaných cestách
- inspection rozhraní (scgms::IID_Drawing_Filter, scgms::IDrawing_Filter_Inspection):
	- lze vyzvednout SVG (string) z inspection rozhraní (Draw)
	- při nové hodnotě New_Data_Available vrátí true, a pak vrací false až do doby, než přijde další nová hodnota
- tady toho moc víc není, tohle budeme předělávat výhledově; nové vykreslování bude třeba jako BP pro někoho jiného na další rok

log filtr
- konfigurace:
	- cesta musí být platná, soubor může ale nemusí existovat
- funkce:
	- při průchodu události se časem přeleje do výstupu - jistojistě po průchodu Shut_Down události, proto to testujte vždy tak, že pošlete nakonec Shut_Down, a až pak ověříte výstup
	- log filtr zaznamenává všechny události, které jím projdou --> N událostí = N řádků v CSV souboru, N+1 včetně hlavičky
	- výstupem je vždy CSV soubor
	- log filtr zaznamenává vždy v pořadí, v jakém zprávy přijdou
- inspection rozhraní (scgms::IID_Log_Filter_Inspection, scgms::ILog_Filter_Inspection):
	- voláním "Push" se vždy má vyzvednout rozdíl proti poslednímu volání
		- je tedy vhodné otestovat, že Push nikdy nevrátí tutéž událost vícekrát
		- a že počet událostí co dovnitř vleze se vždy propíše 1:1 do toho co vrací Push

log replay filtr
- konfigurace:
	- cesta musí být platná, soubor/složka musí existovat (neexistující soubor = chyba)
		- pozn.: pokud je to soubor, bude se přehrávat jen ten; pokud je to složka, přehrajou se všechny soubory v ní
- funkce:
	- pokud soubor není CSV/nelze ho naparsovat, pošle se událost s error zprávou
	- počet řádků (všech) CSV (bez hlavičky) = kolik událostí se pošle z filtru ven
	- pokud je nakonfigurována flaga "emit shutdown message", tak se na konci po všem sám pošle ještě Shut_Down
	- pokud je vstupem složka, přehrají se všechny platné log soubory v ní
	- pokud je vstupem složka, a je nakonfigurovaný parametr "interpret filename as segment id", přepíšou se segment_id ve všech souborech na unikátní (tohle nemáte moc jak ověřit jinak, než na tu unikátnost - tedy kolik souborů, tolik alespoň segmentů na výstupu)

signal error filtr
- konfigurace:
	- reference a error signal musí existovat (když alespoň jeden z nich neexistuje, vrací chybu)
	- metrika musí existovat
	- metric level required je >= 0
	- output CSV file pokud je cesta zadaná, musí být platná
- funkce:
	- akumuluje chybu signálu a jeho referenčního signálu (např. vypočtený vs naměřený) - v podstatě wrapper nad metrikou
	- testovat se bude spíš inspection
	- pokud je nastaveno "emit error as signal", pokaždé když je nová hodnota chybového signálu, je poslána jako signál (event)
- inspection rozhraní (scgms::IID_Signal_Error_Inspection, scgms::ISignal_Error_Inspection):
	- vrací správné signal stats po požádání (průměrná chyba, odchylka, ...) (Calculate_Signal_Error)
	- Promise_Metric
		- pokud je nastaven defer_to_dtor, naplní metric_value až po svém ukončení
		- pokud ne, vypočte metriku okamžitě

calculate filtr
- ještě dovymyslíme, zatím otestujte genericky a dle uvážení konfiguraci

masking filtr
- konfigurace:
	- signál musí existovat
	- bitmaska musí být velikosti 8, 16, 24, ... 64 bitů (násobek 8), ne jiná; jinak vrací chybu
- funkce:
	- dle bitmapy mění "Level" na "Masked_Level" - chce to tedy ověřit, zda to dělá správně a zda se vzor správně opakuje

mapping filtr
- konfigurace:
	- zdrojový signál je buď platný signál nebo "all signals" (scgms::signal_All)
	- cílový signál je buď platný signál nebo "null signál" (scgms::signal_Null)
- funkce:
	- mapuje výhradně jen zdrojový signál na cílový, pokud je zdrojový konkrétní signál (ne "all") a cílový není "null"
	- pokud je cílový signal "null signál", tak událost zahazuje (nedostane se ven z filtru - jakmile se vrátí z metody Execute, na druhé straně event prostě nevypadne)

signal generator
- ještě dovymyslíme, zatím otestujte genericky a dle uvážení konfiguraci

impulse response filtr
- konfigurace:
	- signál musí být platný signál (ne Null ani All)
	- response window je kladný nenulový čas
- funkce:
	- tohle se bude testovat blbě, je to v podstatě výpočet klouzavého průměru
		- šlo by tam poslat nějaký průběh signálu, kde si klouzavý průměr předvypočtete a pak ověříte, zda to sedí s nějakou malou deltou

feedback sender filtr
- lze testovat pouze v kombinaci s příslušným signal generator filtrem, to také ještě dovymyslíme

decoupling filtr
- konfigurace:
	- signály musí existovat
	- condition lze zadat jako booleovskou formuli (tady ještě dodám specifika), pro teď netestujte
	- output csv file opět musí být nějaká platná cesta
- funkce:
	- základ funguje jako mapping filtr, pokud je "remove from source" true
	- pokud "remove from source" je false, tak se ven dostává původní signál i nový namapovaný (tedy se duplikuje pod jiným ID)
	- statistiky průchodu jsou ukládány do CSV souboru
- víc bych ho asi neřešil

