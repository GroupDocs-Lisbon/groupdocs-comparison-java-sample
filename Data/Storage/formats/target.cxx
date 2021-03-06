#include <log4cplus/layout.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/internal/internal.h>
#include <ostream>
#include <iomanip>


namespace log4cplus
{

void
formatRelativeTimestamp (log4cplus::tostream & output,
    log4cplus::spi::InternalLoggingEvent const & event)
{
    auto const duration
        = event.getTimestamp () - getTTCCLayoutTimeBase ();
    output << helpers::chrono::duration_cast<
                  helpers::chrono::duration<long long, std::milli>>(
                      duration).count ();
}

//
//
//


Layout::Layout ()
    : llmCache(getLogLevelManager())
{ }


Layout::Layout (const log4cplus::helpers::Properties&)
    : llmCache(getLogLevelManager())
{ }


Layout::~Layout() = default;

SimpleLayout::SimpleLayout () = default;


SimpleLayout::~SimpleLayout() = default;


void
SimpleLayout::formatAndAppend(log4cplus::tostream& output,
                              const log4cplus::spi::InternalLoggingEvent& event)
{
    output << llmCache.toString(event.getLogLevel())
           << LOG4CPLUS_TEXT(" - ")
           << event.getMessage()
           << LOG4CPLUS_TEXT("\n");
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::TTCCLayout ctors and dtor
///////////////////////////////////////////////////////////////////////////////

  TTCCLayout::TTCCLayout(bool use_gmtime_, bool thread_printing_,
      bool category_prefixing_, bool context_printing_)
    : use_gmtime(use_gmtime_)
    , thread_printing (thread_printing_)
    , category_prefixing (category_prefixing_)
    , context_printing (context_printing_)
{
}


TTCCLayout::TTCCLayout(const log4cplus::helpers::Properties& properties)
    : Layout(properties)
    , dateFormat(properties.getProperty (LOG4CPLUS_TEXT("DateFormat"),
            internal::empty_str))
{
    properties.getBool (use_gmtime, LOG4CPLUS_TEXT("Use_gmtime"));
    properties.getBool (thread_printing, LOG4CPLUS_TEXT("ThreadPrinting"));
    properties.getBool (category_prefixing, LOG4CPLUS_TEXT("CategoryPrefixing"));
    properties.getBool (context_printing, LOG4CPLUS_TEXT("ContextPrinting"));
}


TTCCLayout::~TTCCLayout() = default;



///////////////////////////////////////////////////////////////////////////////
// log4cplus::TTCCLayout public methods
///////////////////////////////////////////////////////////////////////////////

void
TTCCLayout::formatAndAppend(log4cplus::tostream& output,
                            const log4cplus::spi::InternalLoggingEvent& event)
{
     if (dateFormat.empty ())
         formatRelativeTimestamp (output, event);
     else
         output << helpers::getFormattedTime(dateFormat, event.getTimestamp(),
             use_gmtime);

     if (getThreadPrinting ())
         output << LOG4CPLUS_TEXT(" [")
                << event.getThread()
                << LOG4CPLUS_TEXT("] ");
     else
         output << LOG4CPLUS_TEXT(' ');

     output << llmCache.toString(event.getLogLevel())
            << LOG4CPLUS_TEXT(' ');

     if (getCategoryPrefixing ())
         output << event.getLoggerName()
                << LOG4CPLUS_TEXT(' ');

     if (getContextPrinting ())
         output << LOG4CPLUS_TEXT("<")
                << event.getNDC()
                << LOG4CPLUS_TEXT("> ");

     output << LOG4CPLUS_TEXT("- ")
            << event.getMessage()
            << LOG4CPLUS_TEXT("\n");
}


bool
TTCCLayout::getThreadPrinting() const
{
    return thread_printing;
}


void
TTCCLayout::setThreadPrinting(bool thread_printing_)
{
    thread_printing = thread_printing_;
}


void
TTCCLayout::setContextPrinting(bool context_printing_)
{
    context_printing = context_printing_;
}


} // namespace log4cplus