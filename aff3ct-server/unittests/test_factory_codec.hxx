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

/*****************************************************************************/
class Test_Factory_Codec : public CxxTest::TestSuite
    {
    public:
        void testMethod( void )
        {
            TS_ASSERT( 1 + 1 > 1 );
            TS_ASSERT_EQUALS( 1 + 1, 2 );
        }
        
        
        
    };

/*****************************************************************************/
#endif /* TEST_FACTORY_CODEC_HXX */

