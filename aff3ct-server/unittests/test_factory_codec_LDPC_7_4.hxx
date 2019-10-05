/*****************************************************************************/
/*
    Copyright esolutions GmbH
    All rights reserved
    Authors:
    Sergej Semenov (EXT)
 * realize an optional as defined in 
 *   [SWS_CM_01033][
 */
/*****************************************************************************/

#ifndef TEST_FACTORY_CODEC_HXX
#define TEST_FACTORY_CODEC_HXX

#include <cxxtest/TestSuite.h>

#include <Factory_Codec.hpp>

using namespace aff3ct;
/*****************************************************************************/
class Test_Factory_Codec_LDPC_7_4 : public CxxTest::TestSuite
    {
        
        
        
        std::unique_ptr<simulation::Codec>  m_codec;
        
        std::list<std::string>              m_paramString = {
                "anystring",
                "-C",
                "LDPC",
                "-K",
                "4",
                "-N",
                "7",
                "--enc-type",
                "LDPC_H",
                "--enc-g-method",
                "IDENTITY",
                "--dec-h-path",
                "codes/ldpc/BCH_7_4_1_strip.alist",
                "--dec-type",
                "BP_FLOODING",
                "--dec-implem",
                "GALA",
                "-i",
                "200"
                };
        
        

    public:
        
        Test_Factory_Codec_LDPC_7_4()
        {
            std::error_code ec;
            m_codec = Factory_Codec::create(m_paramString, ec, std::cout);
        }
                
        ~Test_Factory_Codec_LDPC_7_4() = default;
        
        void testCreateCodecLDPC() 
        {
            TS_ASSERT(m_codec); //Check that codec is created
        }
        
        void testKN()
        {
            TS_ASSERT(m_codec->getN() == 7);
            TS_ASSERT(m_codec->getK() == 4);
        }
        
        void testParams()
        {
            factory::Codec_Generic::parameters  &params = m_codec->creator->getParams();
            
            TS_ASSERT(params.cdc->enc->N_cw == 7);
            TS_ASSERT(params.cdc->enc->K == 4);
            
        }
        
        void testEncode()
        {
            int N = 7;  
            int n_frames = 3;   
            
            TS_ASSERT(m_codec); //Check that codec is created
        
            /* ENCODE TEST STARTS */
            std::vector<int32_t> data = { 1, 1, 1, 0, 
                                          0, 0, 0, 0, 
                                          1, 1, 1, 1}; 
            std::vector<int32_t> codewords(n_frames * N);
            
            std::vector<int32_t> expected = {0, 1, 0, 1, 1, 1, 0,
                                             0, 0, 0, 0, 0, 0, 0,
                                             1, 1, 1, 1, 1, 1, 1};

            m_codec->encode(&data[0], &codewords[0], n_frames);  
            
            TS_ASSERT(codewords == expected);
        }

        void testIsCodeword()
        {         
            TS_ASSERT(m_codec); //Check that codec is created
            
            std::vector<int32_t> codeword = {0, 1, 0, 1, 1, 1, 0};
            TS_ASSERT(m_codec->is_codeword(&codeword[0]) == true);
            
            std::vector<int32_t> non_codeword = {0, 1, 1, 1, 1, 1, 0};
            TS_ASSERT(m_codec->is_codeword(&non_codeword[0]) == false);
        }
        
        void testMethod( void )
        {
            TS_ASSERT( 1 + 1 > 1 );
            TS_ASSERT_EQUALS( 1 + 1, 2 );
        }
        
        void testCodecType(void)
        {
            simulation::CodecType type= m_codec->getCodecType();    
            TS_ASSERT(type == simulation::CodecType::Type_SISO_SIHO);
        }
        void testDecodeSIHO()
        {
            std::vector<int32_t> vector  = {1, 1, 1, 0};
            //std::vector<int32_t> codeword  = {0, 1, 0, 1, 1, 1, 0};
            std::vector<float> recieved_llr = {-0.92,  1.68,  2.85,  0.78,  1.63,  3.21, -0.19};

            std::vector<int32_t> decoded(vector.size());
            
            try {
                m_codec->decodeSIHO(&recieved_llr[0], &decoded[0], 1);
            } catch (const std::exception& e) {
                std::cout << "SIHO decoder failed: " << e.what() << std::endl;
            }
            
            TS_ASSERT(decoded == vector);
        }
        
        void testDecodeHIHO()
        {
            bool bFailed = false;
            std::vector<int32_t> codeword       = {0, 1, 0, 1, 1, 1, 0};
            std::vector<int32_t> non_codeword   = {0, 1, 1, 1, 1, 1, 0};
            
            std::vector<int32_t> decoded(codeword.size());
            try {
                //HIHO shall fail for LDPC
                m_codec->decodeHIHO(&non_codeword[0], &decoded[0], 1);
            } catch (const std::exception& e) {
                bFailed = true;
            }
            TS_ASSERT(bFailed);
        }
        
        void testDecodeSISO()
        {
            bool bFailed = false;
            std::vector<float> recieved_llr = {-0.92,  1.68,  2.85,  0.78,  1.63,  3.21, -0.19};

            std::vector<float> decoded(recieved_llr.size());
            
            try {
                m_codec->decodeSISO(&recieved_llr[0], &decoded[0], 1);
            } catch (const std::exception& e) {
                bFailed = true;
            }
            
            TS_ASSERT(bFailed);
        }
        
    };

/*****************************************************************************/
#endif /* TEST_FACTORY_CODEC_HXX */

