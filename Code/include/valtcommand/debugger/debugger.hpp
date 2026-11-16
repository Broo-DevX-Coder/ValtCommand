/**
 * @file debugger.hpp
 * @brief Debugger declarations: event tracking and breakpoint management.
 *
 * This header declares the runtime event tracker (@ref vltc::DevTools::event,
 * @ref vltc::DevTools::EventsT) and the @ref vltc::DevTools::Debugger class
 * that emits events and pauses execution on breakpoints.
 */

#pragma once

#include "valtcommand/api_export.hpp"

/// @name Local includes
/// @{
#include "valtcommand/scope.hpp"
/// @}

/// @name Library includes
/// @{
#include <functional>
/// @}

/**
 * @brief Root namespace of the ValtCommand language.
 */
namespace vltc {

/**
 * @brief Developer tools: event tracking and debugging utilities.
 */
namespace DevTools {

    /**
     * @brief Levels (categories) of events.
     */
    enum class VALTC_API EventsT {
        Info,     ///< General information.
        Analysis, ///< Semantic analysis step (verifies all code before running).
        Runtime,  ///< Events raised while the program is running.
        Debug,    ///< Debugger events (breakpoints, ...).
        Warning,  ///< A warning.
        Error     ///< An error that stopped the program.
    };

    /**
     * @brief Convert an event type to its string name.
     * @param tegory The event type to convert.
     * @return The string representation of @p tegory.
     */
    VALTC_API std::string event_type_to_string(EventsT tegory);

    /**
     * @brief A single event emitted by the runtime or the debugger.
     */
    struct VALTC_API event {
        EventsT type;                ///< The level of the event.
        std::string message;         ///< The message of the event.
        size_t line;                 ///< The line where the event happened.
        Scopes::Scope* curent_scope; ///< The scope of the event's line.
        std::string file;            ///< The file where the event happened.

        /**
         * @brief Format the event as a printable string.
         * @return A string like `[Type] line:N -> message` ending with a newline.
         */
        std::string get_str() {
            return fmt::format("[{}] line:{} -> {}\n", event_type_to_string(type), line, message);
        };
    };

    /**
     * @brief A breakpoint set on a specific line (and optionally a file).
     */
    struct VALTC_API breakPointType {
        private:
            size_t line;       ///< The line of the breakpoint.
            fs::path file;  ///< File containing the breakpoint; empty matches any file.
            bool avilable = true; ///< Whether the breakpoint is enabled and can stop execution.

            bool temp = false; ///< Whether the breakpoint is temporary (one-shot).
            bool used = false; ///< Whether the (temporary) breakpoint has already been used.

        public:
            /**
             * @brief Construct a breakpoint.
             * @param l_ The line of the breakpoint.
             * @param file_path The file containing the breakpoint; empty matches any file.
             * @param temporary Whether the breakpoint is temporary.
             */
            breakPointType(size_t l_, const fs::path& file_path = "", bool temporary = false)
                : line(l_), file(file_path), temp(temporary) {}

            /**
             * @brief Enable or disable the breakpoint.
             * @param av_ `true` to make the breakpoint available.
             */
            void set_av__(bool av_){avilable = av_;}

            /**
             * @brief Mark the breakpoint as used if it is temporary.
             */
            void use(){used=temp;}

            /**
             * @brief Check whether the breakpoint has been used.
             * @return `true` if it was used.
             */
            bool is_used() const {return used;}

            /**
             * @brief Get the file of the breakpoint.
             * @return The file path (empty means any file).
             */
            const fs::path& get_file() const { return file; }

            /**
             * @brief Check whether the breakpoint can still stop execution.
             * @return `true` if it is available and not used.
             */
            explicit operator bool() const {return avilable && !used;}

            /**
             * @brief Get the line of the breakpoint.
             * @return The breakpoint's line number.
             */
            explicit operator size_t() const {return line;}
    };


    /**
     * @brief The debugger: tracks runtime events and handles breakpoints.
     */
    class VALTC_API Debugger {
        public:
            /// Callback for runtime tracker events: receives the event and the output stream.
            using EmitSlotType = std::function<void(event,std::ostream&)>;
            /// Callback called when execution pauses: receives the debugger and the current scope.
            using StopSlotType = std::function<void(Debugger&, Scopes::Scope*)>;

        private:
            using emitSlotType = EmitSlotType;
            using stopSlotType = StopSlotType;

        private:

            /// @name General
            /// @{

            bool debug_mode = false; ///< Whether the debugger is enabled.
            std::ostream* output_stream = &std::cout; ///< Output stream of all language code (`std::cout` or any other stream).

            /// @}

            /// @name Runtime tracker
            /// @{

            std::vector<event> events_list; ///< All events saved so far.
            size_t print_pos = 0;           ///< Position of the next event to print.

            /**
             * @brief Default emit callback: print an event's string.
             * @param event The event to print.
             * @param ostrm The stream to print to.
             */
            static void print_event(event event, std::ostream& ostrm);
            emitSlotType emit_slot = print_event; ///< The current emit callback.

            /// @}

            /// @name Breakpoints
            /// @{

            std::vector<breakPointType> breakpoints; ///< The list of breakpoints.
            size_t curent_line;       ///< The number of the most recently emitted line.
            fs::path curent_file;  ///< File of the most recently emitted event.

            /**
             * @brief Default stop callback: shows `Enter to continue ...` when a breakpoint is hit.
             * @param debuger The debugger that stopped.
             * @param scope The scope where execution stopped.
             */
            static void stop_slot(Debugger& debuger, Scopes::Scope* scope);
            stopSlotType stopCodeSlot = stop_slot; ///< The callback called when the code stops.

            /// @}

        public:

            /// @name General
            /// @{

            /**
             * @brief Default constructor (output goes to `std::cout`).
             */
            Debugger() = default;

            /**
             * @brief Construct a debugger with a custom output stream.
             * @param ostream The output stream to use.
             */
            Debugger(std::ostream& ostream){output_stream = &ostream;}

            /**
             * @brief Set the output stream of the debugger.
             * @param ostream The new output stream.
             */
            void set_ostream(std::ostream& ostream){output_stream=&ostream;}

            /**
             * @brief Get the output stream of the debugger.
             * @return Pointer to the current output stream.
             */
            const std::ostream* get_ostream() const {return output_stream;}

            /// @}

            /// @name Runtime tracker
            /// @{

            /**
             * @brief Emit an event.
             * @param event The event to emit.
             */
            void emit(event event);

            /**
             * @brief Set the runtime tracker callback.
             * @param func The callback called for each emitted event.
             */
            void set_emit_event_slot(EmitSlotType func);

            /**
             * @brief Get the string of the next event after the current position.
             * @return The next event's string.
             */
            std::string get_next_event_str();

            /**
             * @brief Get the strings of all events.
             * @return All events' strings.
             */
            std::string get_all_events_strs();

            /// @}

            /// @name Breakpoints
            /// @{

            /**
             * @brief Add several breakpoints at once.
             * @param points_buffer The lines of the breakpoints to add.
             * @param FilePath The file of the breakpoints; empty matches any file.
             */
            void add_break_points(
                const std::vector<size_t>& points_buffer,
                const fs::path& FilePath = ""
            );

            /**
             * @brief Check the current line and file against the breakpoints.
             * @param line The current line.
             * @param scope The current scope.
             * @param FilePath The current file.
             */
            void check_break_point(size_t line, Scopes::Scope* scope, const fs::path& FilePath = "");

            /**
             * @brief Set the callback used when execution pauses.
             * @param slot The callback to call on pause.
             */
            void set_stop_slot(StopSlotType slot);

            /// @}

            /// @name Managing debugging
            /// @{

            /**
             * @brief Continue execution after a pause.
             */
            void continue_exec();

            /**
             * @brief Move to the next line.
             */
            void next_line();

            /**
             * @brief Exit the program.
             */
            void exit();

            /// @}
    };
}

} // namespace vltc
