__kernel void HelloWorld(__global char* data)
{
    data[0] = 'H';
    data[1] = 'e';
    data[2] = 'l';
    data[3] = 'l';
    data[4] = 'o';
    data[5] = ' ';
    data[6] = 'w';
    data[7] = 'o';
    data[8] = 'r';
    data[9] = 'l';
    data[10] = 'd';
    data[11] = '!';
    data[12] = '\n';
}





/*
//stolen code what does the inamge drawing
// We will sample using normalized coordinates.
        // Because we will sample exact values, we can choose nearest filtering.
        const sampler_t kSampler =
            CLK_NORMALIZED_COORDS_FALSE
            | CLK_ADDRESS_CLAMP_TO_EDGE
            | CLK_FILTER_NEAREST;

        // Stereogram-generating kernel.
        __kernel void Stereogram(
            __write_only image2d_t pOffsetImage ,
            __read_only image2d_t pDepthImage
            )
        {
            // Private buffer to hold last image offset.
            float lBuffer[ kPatternWidth ];

            const int2 lOutputDim = get_image_dim( pOffsetImage );
            const int  lRowPos = get_global_id( 0 );
            if ( lRowPos >= lOutputDim.y )
                return;

            // First copy direct values.
            for ( int i = 0 ; i < kPatternWidth; ++i )
            {
                const float lValue = ( i / (float) kPatternWidth );
                // We copy them in the temporary buffer from which we will fetch upcoming offsets.
                lBuffer[ i ] = lValue;

                // ... and we also output it in the first band of the image.
                const int2 lOutputPos = { i , lRowPos };
                write_imagef( pOffsetImage , lOutputPos , (float4) lValue );
            }

            // Then actually generate offsets based on depth.
            for ( int i = kPatternWidth ; i < lOutputDim.x; ++i )
            {
                const int2 lLookupPos = { i - kPatternWidth , lRowPos };
                const float4 lDepth = read_imagef( pDepthImage , kSampler , lLookupPos );
                const float  lOffset = kMaxOffset * ( 1 - lDepth.x );
                const float  lPos  = i + lOffset;
                const int    lPos1 = ( (int) ( lPos ) );
                const int    lPos2 = ( lPos1 + 1 );
                const float  lFrac = lPos - lPos1;
                const float  lValue1 = lBuffer[ lPos1 % kPatternWidth ];
                const float  lValue2 = lBuffer[ lPos2 % kPatternWidth ];
                const float  lValue = 1 + lValue1 + lFrac * ( lValue2 - lValue1 );

                // Update private buffer.
                lBuffer[ i % kPatternWidth ] = lValue;

                // Update output image.
                const int2 lOutputPos = { i , lRowPos };
                write_imagef( pOffsetImage , lOutputPos , (float4) lValue );
            }
        };
        */