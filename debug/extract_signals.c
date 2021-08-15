#include<stdint.h>
#include<stdio.h>
#include<string.h>
//main entrypoint
typedef enum {
    COM_BOOLEAN,
    COM_UINT8,
    COM_UINT16,
    COM_UINT32,
    COM_UINT64,
    COM_UINT8_N,
    COM_UINT8_DYN,
    COM_SINT8,
    COM_SINT16,
    COM_SINT32,
    COM_SINT64
} Com_SignalType;

typedef enum {
    COM_BIG_ENDIAN,
    COM_LITTLE_ENDIAN,
    COM_OPAQUE
} ComSignalEndianess_type;

// typedef uint32 uint32_t_t;
// typedef uint8 uint8_t;
// typedef uint16 uint16_t;

void Com_Misc_ReadSignalDataFromPdu (
        const uint8_t *comIPduDataPtr,
        uint16_t bitPosition,
        uint16_t bitSize,
        ComSignalEndianess_type endian,//0 ->big endian  1-> little endian
        Com_SignalType signalType,
        uint8_t *SignalData) {
    // SchM_Enter_Com_EA_0();
    if ((endian == COM_OPAQUE) || (signalType == COM_UINT8_N)) {
        /* Aligned opaque data -> straight copy*/
        /* @req COM472 */
        memcpy(SignalData, &comIPduDataPtr[bitPosition/8], bitSize / 8);
        // SchM_Exit_Com_EA_0();
    } else if ((signalType != COM_UINT64) && (signalType != COM_SINT64)) {
        /* Unaligned data and/or endian-ness conversion*/
        uint32_t pduData = 0ULL;
        if(endian == COM_BIG_ENDIAN) {
            printf("big endian\n");
            // uint32_t lsbIndex = ((bitPosition ^ 0x7u) + bitSize - 1) ^ 7u; /* calculate lsb bit index. This could be moved to generator*/
            uint32_t lsbIndex = bitPosition; /* calculate lsb bit index. This could be moved to generator*/
            const uint8_t *pduDataPtr = ((&comIPduDataPtr [lsbIndex / 8])-3); /* calculate big endian ptr to data*/
            uint8_t bitShift = lsbIndex % 8;
            for(int8_t i = 0; i < 4; i++) {
                pduData = (pduData << 8) | pduDataPtr[i];
            }
            pduData >>= bitShift;
            if((32 - bitShift) < bitSize) {
                pduData |= (uint32_t)pduDataPtr[-1] << (32u - bitShift);
            }
        } else if (endian == COM_LITTLE_ENDIAN) {
            printf("little endian\n");
            uint32_t lsbIndex = bitPosition;
            const uint8_t *pduDataPtr = &comIPduDataPtr[(bitPosition/8)];
            uint8_t bitShift = lsbIndex % 8;
            printf("for loop little\n");
            for(int8_t i = 3; i >= 0; i--) {
                printf("loop_cnt:%d\n", i);
                pduData = (pduData << 8) | pduDataPtr[i];
            }
            printf("line65\n");
            pduData >>= bitShift;
            if((32 - bitShift) < bitSize) {
                pduData |= (uint32_t)pduDataPtr[4] << (32u - bitShift);
            }
        } else {
            // ASSERT(0);
        }
        printf("padding bits\n");
        // SchM_Exit_Com_EA_0();
        uint32_t mask = 0xFFFFFFFFUL >> (32u - bitSize); /* calculate mask for SigVal */
        pduData &= mask; /* clear bit out of range */
        uint32_t signmask = ~(mask >> 1u);
        switch(signalType) {
        case COM_SINT8:
            if(0 < (pduData & signmask)) {
                pduData |= signmask; /* add sign bits */
            }
            /* sign extended data can be written as uint*/
            /*lint -e{734} pointer cast and assigning 32bit pduData to 8bit SignalData is required operation*/
            *SignalData = pduData;
            break;
        case COM_BOOLEAN:
            /*lint -e{734} pointer cast and assigning 32bit pduData to 8bit SignalData is required operation*/
            *SignalData = pduData;
            break;
        case COM_UINT8:
            /*lint -e{734} pointer cast and assigning 32bit pduData to 8bit SignalData is required operation*/
            *SignalData = pduData;
            break;
        case COM_SINT16:
            if(0 < (pduData & signmask)) {
                pduData |= signmask; /* add sign bits*/
            }
            /* sign extended data can be written as uint*/
            /*lint -e{927} -e{734} -e{826} pointer cast and assigning 32bit pduData to 16bit SignalData is required operation*/
            *(uint16_t*)SignalData = pduData;
            break;
        case COM_UINT16:
            /*lint -e{927} -e{734} -e{826} pointer cast and assigning 32bit pduData to 16bit SignalData is required operation*/
            *(uint16_t*)SignalData = pduData;

            break;
        case COM_SINT32:
            if(0 < (pduData & signmask)) {
                pduData |= signmask; /* add sign bits*/
            }
            /*sign extended data can be written as uint */
            /*lint -e{927} -e{826} pointer cast is required operation*/
            *(uint32_t*)SignalData = pduData;
            break;
        case COM_UINT32:
            /*lint -e{927} -e{826} pointer cast is required operation*/
            *(uint32_t*)SignalData = pduData;
            break;
        case COM_UINT8_N:
        case COM_UINT8_DYN:
        case COM_SINT64:
        case COM_UINT64:
            // ASSERT(0);
            break;
        default : break;
        }
    } else {
        /* Call separate function for 64bits values.*/
        /* Note: SchM_Exit_Com_EA_0 is called from within called function */
        // Com_Misc_ReadSignalDataFromPdu64Bits(comIPduDataPtr, bitPosition,
        //         bitSize, endian, signalType, SignalData);
	}
    // printf("%x\n",SignalData)
		}
		
int main(void){
/*
        const uint8_t *comIPduDataPtr,
        uint16_t bitPosition,
        uint16_t bitSize,
        ComSignalEndianess_type endian,//0 ->big endian  1-> little endian
        Com_SignalType signalType,
        uint8_t *SignalData

*/
	uint8_t pdu[] = {0x12,0x32,0x56,0xaa,0xbb,0xcc,0xdd};
	uint16_t bitposition = 8;
	uint16_t bitsize	= 16;
	ComSignalEndianess_type endian = 0;
	Com_SignalType type = COM_UINT16;
	uint8_t out[8] ={0x00};
    printf("test start\n");
	Com_Misc_ReadSignalDataFromPdu(pdu,bitposition,bitsize,endian,type,out);
	printf("%x\n",out[0]);
	printf("%x\n",out[1]);
	return 0;
}
		
