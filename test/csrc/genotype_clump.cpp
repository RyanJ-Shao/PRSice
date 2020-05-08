#include "catch.hpp"
#include "genotype.hpp"
#include "mock_binaryplink.hpp"
#include "mock_genotype.hpp"

TEST_CASE("Sort by p")
{
    mockGenotype geno;
    Reporter reporter("log", 60, true);
    geno.set_reporter(&reporter);
    std::vector<SNP> input = {SNP("rs4567", 1, 1000, "A", "C", 0, 0.06, 0, 0),
                              SNP("rs3456", 2, 1000, "A", "C", 0, 0.05, 0, 0),
                              SNP("rs3455", 2, 1000, "A", "C", 0, 0.06, 0, 0),
                              SNP("rs3452", 2, 1003, "A", "C", 0, 0.06, 0, 0),
                              SNP("rs3457", 2, 1003, "A", "C", 0, 0.06, 0, 0)

    };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(input.begin(), input.end(), g);
    for (auto&& snp : input) { geno.load_snp(snp); }
    REQUIRE_NOTHROW(geno.sort_by_p());
    auto idx = geno.sorted_p_index();
    std::vector<std::string> expected_order = {"rs4567", "rs3456", "rs3455",
                                               "rs3452", "rs3457"};
    auto res = geno.existed_snps();
    REQUIRE(res.size() == expected_order.size());
    for (size_t i = 0; i < res.size(); ++i)
    { REQUIRE(res[idx[i]].rs() == expected_order[i]); }
}
TEST_CASE("Build Clump window")
{
    mockGenotype geno;
    Reporter reporter("log", 60, true);
    geno.set_reporter(&reporter);
    std::vector<SNP> input = {SNP("rs1", 1, 10, "A", "C", 0, 10),
                              SNP("rs5", 1, 10, "A", "C", 0, 13),
                              SNP("rs3", 1, 10, "A", "C", 1, 10),
                              SNP("rs4", 1, 20, "A", "C", 1, 10),
                              SNP("rs10", 1, 25, "A", "C", 1, 10),
                              SNP("rs6", 1, 40, "A", "C", 1, 10),
                              SNP("rs7", 1, 70, "A", "C", 1, 10),
                              SNP("rs8", 3, 70, "A", "C", 1, 10)};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(input.begin(), input.end(), g);
    for (auto&& snp : input) { geno.load_snp(snp); }
    REQUIRE_NOTHROW(geno.build_clump_windows(10));
    auto res = geno.existed_snps();
    std::vector<std::string> expected_order = {"rs1",  "rs5", "rs3", "rs4",
                                               "rs10", "rs6", "rs7", "rs8"};
    // window size only need to cover # SNP before
    REQUIRE(geno.max_window() == 4);
    REQUIRE(res.size() == expected_order.size());
    for (size_t i = 0; i < res.size(); ++i)
    {
        REQUIRE(res[i].rs() == expected_order[i]);
        switch (i)
        {
        case 0:
        case 1:
        case 2:
            REQUIRE(res[i].up_bound() == 4);
            REQUIRE(res[i].low_bound() == 0);
            break;
        case 3:
            REQUIRE(res[i].up_bound() == 5);
            REQUIRE(res[i].low_bound() == 0);
            break;
        case 4:
            REQUIRE(res[i].up_bound() == 5);
            REQUIRE(res[i].low_bound() == 3);
            break;
        case 5:
            REQUIRE(res[i].up_bound() == 6);
            REQUIRE(res[i].low_bound() == 5);
            break;
        case 6:
            REQUIRE(res[i].up_bound() == 7);
            REQUIRE(res[i].low_bound() == 6);
            break;
        case 7:
            REQUIRE(res[i].up_bound() == 8);
            REQUIRE(res[i].low_bound() == 7);
            break;
        }
    }
}


// update_index_tot function (might need to use plink and predefined data)
// get_r2 function (again, might need to use predefined data and reference to
// plink)

TEST_CASE("R2 calculation")
{
    // can just use plink format as we know the bgen read is correct (when there
    // isn't any non-founder samples)
    // we need to generate around 10 SNPs with different genotype adn calculate
    // their expected R2 (calculated from PLINK)
    // Maybe use something like the 1000 genome data?
    const size_t n_sample = 250;
    std::vector<std::vector<uintptr_t>> dummy_input = {
        {18442170087889698815ul, 18445547801835994110ul, 13470264286370594747ul,
         13830554451359760127ul, 18446744073709535231ul, 18139162224121282475ul,
         18427599012157718527ul, 4499201245315071ul},
        {18446744069413519359ul, 13817043639324360703ul, 18446744073709547515ul,
         17293752196063559679ul, 18369901405063348223ul, 18442240473746636799ul,
         18374686479666346943ul, 4503599610523644ul},
        {18446744073709551615ul, 18410715241793961983ul, 18446744073709551611ul,
         17293752200358526975ul, 18302628885625307135ul, 18442240473746636799ul,
         18374686479665330047ul, 4503599627362301ul},
        {18437596101935628287ul, 18301362246089440125ul, 8790990188667994041ul,
         9214364832768062943ul, 17868031521323859967ul, 18409448569970752875ul,
         6898381474990915581ul, 2247401163193855ul},
        {18428725277136780287ul, 18338657647755984895ul, 18446743781651513343ul,
         17293822551922835387ul, 18298125011120029695ul, 17293822569102442494ul,
         18442161030002833271ul, 4186665396461565ul},
        {18446744065118535679ul, 9223371899415822335ul, 18446744073709543423ul,
         18446744065119617023ul, 17870001846429417471ul, 18446744073709551615ul,
         9223369837831520247ul, 4501400587197951ul},
        {15775809120453242858ul, 17000999510312927118ul, 18374629093472713466ul,
         14679763081484022446ul, 12371387896928185022ul, 18422748331592428479ul,
         17148506438322268907ul, 4485988847860479ul},
        {15762316914735376559ul, 12664051714702175231ul, 13816972188210671599ul,
         18302536440487866099ul, 17221360061636411386ul, 17577267572954627003ul,
         18428659305042407423ul, 4502203762208767ul},
        {18442239357323313151ul, 18446462581552971775ul, 13834987407365111807ul,
         18445336680571322366ul, 18374667787906838527ul, 13826895280953278206ul,
         18158513696345620159ul, 3360107533369343ul},
        {18078573256838408637ul, 16980305538350514153ul, 4233283777687644886ul,
         4591977148665707496ul, 11397779609568849871ul, 12583688906873548294ul,
         9498091266898722482ul, 3146368150055678ul},
        {18369901405017209855ul, 18445618173802692607ul, 13835058050987196415ul,
         18446744072635809791ul, 13762999361665429247ul, 18440827601640487935ul,
         18063938105382731519ul, 4486007441260223ul},
        {18374683176773353471ul, 18158213462163914479ul, 3454260913923616750ul,
         18086385733701991358ul, 18373541887794470654ul, 17288967057034952702ul,
         18140428930304179391ul, 4204253287460591ul},
        {3668942357107026666ul, 16924536989979019483ul, 4593305753104657390ul,
         12267509324945799932ul, 17188806748070183470ul, 13511637314131129023ul,
         18067891123399609036ul, 258572277154346ul},
        {12965568656720260846ul, 18445313321250818271ul, 18428443518707941375ul,
         12303548307824688126ul, 18441957538547057343ul, 13524305200268442559ul,
         18356302095499389902ul, 844114568127414ul},
        {18441111206846263022ul, 4607177539649010655ul, 17270178597960678394ul,
         18373066830055927743ul, 12681907573066825727ul, 13474466619882192634ul,
         17270178648241995707ul, 4414448923820030ul},
        {9221100445829693439ul, 13834899588168679391ul, 6773413285512343423ul,
         18446744069414584060ul, 17221729515814649855ul, 17869720371452674047ul,
         13474629207998848511ul, 4502912357039607ul},
        {16140901064493760511ul, 18446741874685247487ul, 18446744073642442751ul,
         18446744073709543423ul, 16140901064495857663ul, 18446744073709027327ul,
         18446744065119617023ul, 4503599618965503ul},
        {5781186985329477864ul, 9469591116002290922ul, 10351464610450335423ul,
         12365205330319739352ul, 18298240404661235245ul, 13513568426446764970ul,
         13322431077835221544ul, 2797977813398330ul},
        {9852446546031561450ul, 14099363035860889323ul, 12658469273848496895ul,
         12393359699976424696ul, 16140526900416802749ul, 18424466778072235950ul,
         17940913933917236010ul, 3371922867502010ul},
        {9852446546031561450ul, 14099363035861413611ul, 12658469273848496895ul,
         12393359699976424696ul, 16140526900416802751ul, 18424501962444324782ul,
         17940915033428863786ul, 3371922867502010ul},
        {18438350602459851562ul, 13833803302777273258ul, 11722495927086136936ul,
         12461172657580850878ul, 12587274727898983150ul, 18212538453600221866ul,
         11749393052362518438ul, 3025764388433151ul},
        {18446735277616529407ul, 8644518609810358271ul, 18446743515363803007ul,
         18445609377692907487ul, 18444492273895866365ul, 13834486171255176159ul,
         18446744073709551607ul, 4362862139015159ul},
        {12605481825216686771ul, 3074449894329150255ul, 18423076802364437503ul,
         12603370624329089195ul, 16987223901729491503ul, 1134617932587511486ul,
         18082211967134786474ul, 994215688027112ul},
        {12609986593074113211ul, 3146586721990864703ul, 18423076733377835775ul,
         12675393231563423915ul, 16968362741827807791ul, 268803086656143038ul,
         18082774917089059755ul, 712740712430568ul},
        {18439988535738890223ul, 18085325805566558139ul, 18446672579682889655ul,
         17221474617692839935ul, 9199831349005905655ul, 18442064169902481407ul,
         12681709179887320059ul, 3940649404460971ul}};


    std::vector<std::vector<double>> expected_r2 = {
        {0.00705706, 8.25051e-05, 0.869208,    0.00686877,  0.00073069,
         0.00345443, 0.0100207,   0.00211663,  1.45889e-07, 0.00387006,
         0.00467646, 0.00468954,  0.0280082,   0.00302556,  0.000453581,
         0.0194231,  0.0108011,   2.28884e-05, 7.52391e-07, 0.0108969,
         0.0012063,  0.0126075,   0.0011813,   0.000398434},
        {0.463584,   0.00032086,  8.12926e-05, 0.124849,    0.00135782,
         0.00369563, 0.000821607, 0.0189643,   0.000908419, 0.00585438,
         0.00206017, 5.6656e-05,  2.65473e-05, 0.0375419,   0.000400946,
         0.00060244, 0.000307863, 0.000537844, 0.00567913,  0.000738677,
         0.0140538,  0.00720283,  0.00241562},
        {0.00708749,  3.37567e-06, 0.000134532, 0.0254002,   0.00184015,
         0.00095259,  0.0108972,   0.012938,    0.000153869, 1.07897e-06,
         0.000185779, 1.70306e-05, 0.000600025, 0.000128722, 0.00103383,
         0.000397182, 0.000537933, 0.000555104, 0.000218873, 0.000833924,
         0.000156628, 0.00382635},
        {0.00433979,  0.000475662, 2.65642e-05, 0.0147727,   0.00162085,
         0.00584206,  0.000498812, 0.00952959,  0.00510422,  0.017757,
         0.00120438,  0.000147996, 0.0483882,   0.000162714, 3.30593e-07,
         1.08239e-05, 0.000195329, 0.000748873, 0.00270667,  0.000333945,
         0.0115114},
        {0.000428388, 0.00244211,  0.000749016, 0.000140048, 0.000216645,
         0.00124009,  0.00226869,  0.00130548,  0.000183332, 0.0127571,
         0.00233217,  0.000364612, 0.000108704, 0.00124302,  0.00210372,
         0.000713404, 0.000695451, 1.37666e-05, 4.57406e-05, 0.0175809},
        {0.0024028, 0.00765518, 0.000630391, 0.00278461, 0.000480837,
         0.000678662, 0.00012805, 0.00631305, 0.00121022, 0.000155836,
         2.91741e-05, 0.00523573, 0.00143958, 0.00153082, 0.00377626,
         8.18853e-05, 0.010524, 0.000431883, 0.00293981},
        {0.0795832, 0.000252209, 0.000474542, 0.0178657, 0.00058133, 0.00832827,
         0.0018056, 0.00389549, 0.0127971, 0.0169584, 0.00117003, 0.0138285,
         0.0145286, 0.000610465, 0.000432053, 0.00565314, 0.000183333,
         0.00267062},
        {0.0017669, 0.0036826, 0.0103827, 0.000323086, 7.28988e-05, 0.000453638,
         0.00770146, 3.28142e-05, 0.00129579, 0.00157305, 0.00105965,
         0.00135376, 0.000479892, 0.00237935, 0.00429054, 0.000221496,
         0.00352185},
        {0.165376, 0.108067, 0.0603994, 0.019895, 0.00304625, 0.00234829,
         0.0133956, 0.000613465, 0.000428748, 0.00096222, 0.00146483, 0.0147593,
         0.0308371, 4.8138e-05, 0.00045609, 2.83599e-05},
        {0.00671866, 0.00325741, 0.000586781, 0.021079, 0.00741339, 0.00334468,
         0.00174926, 0.000974137, 0.000193834, 3.53975e-06, 0.00566243,
         0.00481355, 0.00236841, 0.00232721, 0.00193259},
        {0.0314197, 0.0487867, 0.00425902, 0.0156533, 0.0169826, 0.000486181,
         0.0036046, 0.00287051, 0.00359979, 0.00083021, 0.000840114,
         0.000197257, 0.00604646, 0.00118779},
        {0.00804237, 0.00440814, 0.00439695, 0.0142788, 0.000564222,
         0.000255027, 0.000678519, 0.000576047, 0.000571833, 0.00158181,
         0.0128451, 0.0112256, 0.00402716},
        {0.329282, 0.167898, 0.00347854, 0.00508578, 0.00129964, 0.00235573,
         0.00343596, 0.00870026, 0.00306371, 0.00065356, 0.00213083,
         0.000799123},
        {0.0541335, 0.00254042, 0.00442473, 0.0134384, 0.00748566, 0.0101635,
         0.0239318, 0.00704692, 0.00560485, 0.00725533, 4.35485e-05},
        {8.45257e-06, 0.031006, 0.00207938, 1.69202e-05, 7.95587e-05, 0.0045795,
         0.000454487, 0.0167655, 0.00776411, 0.00257797},
        {0.000291971, 0.0051839, 0.0156142, 0.0167249, 0.00739164, 0.000511542,
         0.00378172, 0.00454379, 0.00626462},
        {0.00780713, 0.0123197, 0.0126044, 0.00377358, 7.89079e-05, 0.000861412,
         0.00144782, 0.000990366},
        {0.473426, 0.462547, 0.185714, 0.0113906, 0.015925, 0.0328072,
         0.011591},
        {0.982085, 0.085499, 0.00585377, 0.031135, 0.0430971, 0.000328091},
        {0.0894888, 0.00567435, 0.0371009, 0.0496297, 0.000742627},
        {0.00698486, 0.0411259, 0.078337, 0.00819144},
        {9.4607e-05, 0.000500084, 0.00171468},
        {0.637221, 0.00036831},
        {0.0286916}};
    // mock_binaryplink plink;
    // plink.gen_fake_bed_from_int(dummy_input, "ld_check", n_sample);

    SECTION("Test R2 calculation")
    {
        mockGenotype geno;
        const uint32_t founder_ctv3 =
            BITCT_TO_ALIGNED_WORDCT(static_cast<uint32_t>(n_sample));
        const uintptr_t founder_ctl2 = QUATERCT_TO_WORDCT(n_sample);
        const uint32_t founder_ctsplit = 3 * founder_ctv3;
        const uintptr_t founder_ctv2 = QUATERCT_TO_ALIGNED_WORDCT(n_sample);
        std::vector<uintptr_t> index_data(3 * founder_ctsplit + founder_ctv3);
        std::vector<uintptr_t> index_tots(6);
        std::vector<uintptr_t> founder_include2(founder_ctv2, 0);
        fill_quatervec_55(static_cast<uint32_t>(n_sample),
                          founder_include2.data());
        for (size_t i = 0; i < dummy_input.size(); ++i)
        {
            geno.test_update_index_tot(founder_ctl2, founder_ctv2, n_sample,
                                       index_data, index_tots, founder_include2,
                                       dummy_input[i].data());
            for (size_t j = i + 1; j < dummy_input.size(); ++j)
            {
                REQUIRE(geno.test_get_r2(founder_ctl2, founder_ctv2,
                                         dummy_input[j].data(), index_data,
                                         index_tots)
                        == Approx(expected_r2[i][j - i - 1]));
            }
        }
    }
}
