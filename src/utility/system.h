#ifndef DSY_SYSTEM_H
#define DSY_SYSTEM_H

#include <cstdint>
#include "tim.h"

namespace daisy
{
/** A handle for interacting with the Core System. 
 ** This includes the Clock tree, MPU, global DMA initialization,
 ** cache handling, and any other necessary global initiailzation
 **
 ** @author shensley
 ** @addtogroup system
 */
class System
{
  public:
    /** Contains settings for initializing the System */
    struct Config
    {
        /** Specifies the system clock frequency that feeds APB/AHB clocks, etc. */
        enum class SysClkFreq
        {
            FREQ_400MHZ,
            FREQ_480MHZ,
        };

        /** Method to call on the struct to set to defaults
         ** CPU Freq set to 400MHz
         ** Cache Enabled 
         ** */
        void Defaults()
        {
            cpu_freq   = SysClkFreq::FREQ_400MHZ;
            use_dcache = true;
            use_icache = true;
        }

        /** Method to call on the struct to set to boost mode:
         ** CPU Freq set to 480MHz
         ** Cache Enabled 
         ** */
        void Boost()
        {
            cpu_freq   = SysClkFreq::FREQ_480MHZ;
            use_dcache = true;
            use_icache = true;
        }

        SysClkFreq cpu_freq;
        bool       use_dcache;
        bool       use_icache;
    };

    /** Describes the different regions of memory available to the Daisy
     * 
     */
    enum MemoryRegion
    {
        INTERNAL_FLASH = 0,
        ITCMRAM,
        DTCMRAM,
        SRAM_D1,
        SRAM_D2,
        SRAM_D3,
        SDRAM,
        QSPI,
        INVALID_ADDRESS,
    };

    System() {}
    ~System() {}

    /** Default Initializer with no input will create an internal config, 
     ** and set everything to Defaults
     */
    void Init();

    /** Configurable Initializer
     ** Initializes clock tree, DMA initializaiton and 
     ** any necessary global inits.
     */
    void Init(const Config& config);

    /** Jumps to the first address of the external flash chip (0x90000000)
     ** If there is no code there, the chip will likely fall through to the while() loop
     ** TODO: Documentation/Loader for using external flash coming soon.
     */
    void JumpToQspi();

    /** \return a uint32_t value of milliseconds since the SysTick started 
    */
    static uint32_t GetNow();

    /** \return a uint32_t of microseconds within the internal timer. */
    static uint32_t GetUs();

    /** \return a uint32_t of ticks at (PCLk1 * 2)Hz 
     ** Useful for measuring the number of CPU ticks 
     ** something is taking.
     ** */
    static uint32_t GetTick();

    /** Blocking Delay that uses the SysTick (1ms callback) to wait.
     ** \param delay_ms Time to delay in ms
     */
    static void Delay(uint32_t delay_ms);

    /** Blocking Delay using internal timer to wait 
     ** \param delay_us Time to ddelay in microseconds */
    static void DelayUs(uint32_t delay_us);


    /** Blocking Delay using internal timer to wait 
     ** \param delay_ticks Time to ddelay in microseconds */
    static void DelayTicks(uint32_t delay_ticks);

    /** Returns the tick rate in Hz with which GetTick() is incremented. */
    static uint32_t GetTickFreq();

    /** Returns the Frequency of the system clock in Hz 
     ** This is the primary system clock that is used to generate
     ** AXI Peripheral, APB, and AHB clocks. */
    static uint32_t GetSysClkFreq();

    /** Returns the frequency of the HCLK (AHB) clock. This is derived
     ** from the System clock, and used to clock the CPU, memory, and
     ** peripherals mapped on the AHB, and APB Bus.
     ** */
    static uint32_t GetHClkFreq();

    /** Returns the frequency of the PCLK1 (APB1) clock
     ** This is used to clock various peripherals, and timers.
     **
     ** It's  important to  note that many timers run on a 
     ** clock twice as fast as the peripheral clock for the timer. 
     ** */
    static uint32_t GetPClk1Freq();

    /** Returns the frequency of the PCLK2 (APB2) clock
     ** This is used to clock various peripherals, and timers.
     **
     ** It's  important to  note that many timers run on a 
     ** clock twice as fast as the peripheral clock for the timer. 
     ** */
    static uint32_t GetPClk2Freq();

    /**
     ** Returns a const reference to the Systems Configuration struct
     */
    const Config& GetConfig() const { return cfg_; }

    /** Returns an enum representing the current (primary) memory space used 
     *  for executing the program.
     */
    static MemoryRegion GetProgramMemoryRegion();

    /** Returns an enum representing the the memory region 
     *  that the given address belongs to.
     *  \param address The address to be checked
     */
    static MemoryRegion GetMemoryRegion(uint32_t address);

  private:
    void   ConfigureClocks();
    void   ConfigureMpu();
    Config cfg_;

    /** One TimerHandle to rule them all
     ** Maybe this whole class should be static.. */
    static TimerHandle tim_;
};

} // namespace daisy

#endif
