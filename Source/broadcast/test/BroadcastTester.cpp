#include <broadcast/broadcast.h>
#include <core/core.h>

using namespace WPEFramework;

void printHelp(){
    printf("Keys to use:\n");
    printf("i -> Initialize \n");
    printf("d -> Deinitialize\n");
    printf("c -> Request a tuner\n");
    printf("t -> Tune\n");
    printf("s -> Switch stream\n");
    printf("? -> This message\n");
    printf("q -> Quit\n");
}

int main(int argc, const char* argv[])
{
    const string configuration = "{ \
        frontends:1, \
        decoders:1, \
        standard: \"DVB\" \
        annex: \"A\"\
        scan:true \
        modus: \"Terrestrial\" \
    }";

    const string information = "0";

    std::list<Broadcast::ITuner*> tuners;
    std::list<Broadcast::ITuner*>::iterator tuner = tuners.begin();

    class streamInfo {
    public:
        streamInfo(uint16_t _frequency, Broadcast::Modulation _modulation, uint32_t _symbolRate, uint16_t _fec, Broadcast::SpectralInversion _si)
            : frequency(_frequency)
            , modulation(_modulation)
            , symbolRate(_symbolRate)
            , fec(_fec)
            , si(_si)
        {
        }

        const uint16_t frequency;
        const Broadcast::Modulation modulation;
        const uint32_t symbolRate;
        const uint16_t fec;
        const Broadcast::SpectralInversion si;
    };

    std::list<streamInfo> streams;

    streams.emplace_back(482, Broadcast::QAM64, 8000000, Broadcast::FEC_1_2, Broadcast::Auto); // Digitenne Enschede
    streams.emplace_back(706, Broadcast::QAM64, 8000000, Broadcast::FEC_1_2, Broadcast::Auto); // Digitenne Amersfoort
    streams.emplace_back(618, Broadcast::QAM64, 8000000, Broadcast::FEC_1_2, Broadcast::Auto); // Digitenne Amsterdam
    streams.emplace_back(474, Broadcast::QAM64, 8000000, Broadcast::FEC_1_2, Broadcast::Auto); // Digitenne Rotterdam

    std::list<streamInfo>::iterator stream = streams.begin();

    char keyPress;

    printf("Ready to start processing events, start with 0 to connect.\n");
    printHelp();
    do {
        keyPress = toupper(getchar());

        switch (keyPress) {
        case 'I': {
            Broadcast::ITuner::Initialize(configuration);
        } break;

        case 'D': {
            Broadcast::ITuner::Deinitialize();
        } break;

        case 'C': {
            Broadcast::ITuner* newTuner = Broadcast::ITuner::Create(information);
            printf("%s:%d %s Create tuner\n", __FILE__, __LINE__, __FUNCTION__);

            if (newTuner != nullptr) {
                printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
                std::list<Broadcast::ITuner*>::iterator entry(std::find(tuners.begin(), tuners.end(), newTuner));
                if (entry == tuners.end()) {
                    printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
                    tuners.push_back(newTuner);
                    printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
                    if (tuner == tuners.end()) {
                        tuner = tuners.begin();
                    }
                }
            } else {
                printf("Sorry, no more tuners availble!\n");
            }
        } break;

        case 'S': {
            printf("%s:%d %s Switch tuner\n", __FILE__, __LINE__, __FUNCTION__);
            if (++tuner == tuners.end()) {
                tuner = tuners.begin();
            }
        } break;

        case 'T': {
            printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
            if (*tuner != nullptr) {
                printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
                if (++stream == streams.end()) {
                    printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
                    stream = streams.begin();
                }
                printf("%s:%d %s Tune: %d %d %d %d %d\n", __FILE__, __LINE__, __FUNCTION__, stream->frequency, stream->modulation, stream->symbolRate, stream->fec, stream->si);
                (*tuner)->Tune(stream->frequency, stream->modulation, stream->symbolRate, stream->fec, stream->si);
                printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
            }
        } break;

        case 'Q':
            break;

        case '?': {
            printHelp();
        } break;

        default: {
            printf("Not known. Press '?' for help.\n");
        } break;
        }
    } while (keyPress != 'Q');

    // Clear the factory we created..
    WPEFramework::Core::Singleton::Dispose();

    printf("\nLeaving the test App !!!\n");

    return 0;
}