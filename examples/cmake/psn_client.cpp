//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 VYV Corporation
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
**/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#include <psn_lib.hpp>
#include <kissnet.hpp>

#include <iostream>

#include <string.h>
#include <unistd.h>
#include <chrono>
#include <thread>

const uint16_t           PORT         = 56565;
static const short    BUFLEN       = 1024;
static const uint32_t TIMEOUT_MSEC = 1000;

namespace kn = kissnet;

int main( void )
{
     char char_buf[BUFLEN];

    //====================================================
    // Init "client" (RECEIVE)

    auto mcast_listen_socket = kissnet::udp_socket();
	mcast_listen_socket.join(kissnet::endpoint("236.10.10.10", 56565));
    
    kn::buffer<1024> recv_buff;

    ::psn::psn_decoder psn_decoder ;
    uint8_t last_frame_id = 0 ;
    int skip_cout = 0 ;

    //====================================================
    // Main loop
    while ( 1 ) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1) );

        printf("Waiting for data...");
        fflush(stdout);

        memset(char_buf, 0, BUFLEN);

        *char_buf = 0;

		auto [received_bytes, status] = mcast_listen_socket.recv(recv_buff);
		const auto from = mcast_listen_socket.get_recv_endpoint();

		std::cout << "Received " << received_bytes << " bytes";
		std::cout << " from: " << from.address << ':' << from.port << '\n';

        if (received_bytes > 0) {

            std::memcpy(char_buf, recv_buff.data(), received_bytes);
            
            psn_decoder.decode( char_buf , BUFLEN ) ;

            // if ( psn_decoder.get_data().header.frame_id != last_frame_id )
            // {
                last_frame_id = psn_decoder.get_data().header.frame_id ;

                const ::psn::tracker_map & recv_trackers = psn_decoder.get_data().trackers ;
                
                // if ( skip_cout++ % 20 == 0 )
                // {
                    ::std::cout << "--------------------PSN CLIENT-----------------" << ::std::endl ;
                    ::std::cout << "System Name: " << psn_decoder.get_info().system_name << ::std::endl ;
                    ::std::cout << "Frame Id: " << (int)last_frame_id << ::std::endl ;
                    ::std::cout << "Frame Timestamp: " << psn_decoder.get_data().header.timestamp_usec << ::std::endl ;
                    ::std::cout << "Tracker count: " << recv_trackers.size() << ::std::endl ;

                    for ( auto it = recv_trackers.begin() ; it != recv_trackers.end() ; ++it )
                    {
                        const ::psn::tracker & tracker = it->second ;

                        ::std::cout << "Tracker - id: " << tracker.get_id() << " | name: " << tracker.get_name() << ::std::endl ;

                        if ( tracker.is_pos_set() )
                            ::std::cout << "    pos: " << tracker.get_pos().x << ", " << 
                                                          tracker.get_pos().y << ", " <<
                                                          tracker.get_pos().z << std::endl ;

                        if ( tracker.is_speed_set() )
                            ::std::cout << "    speed: " << tracker.get_speed().x << ", " << 
                                                            tracker.get_speed().y << ", " <<
                                                            tracker.get_speed().z << std::endl ;

                        if ( tracker.is_ori_set() )
                            ::std::cout << "    ori: " << tracker.get_ori().x << ", " << 
                                                          tracker.get_ori().y << ", " <<
                                                          tracker.get_ori().z << std::endl ;

                        if ( tracker.is_status_set() )
                            ::std::cout << "    status: " << tracker.get_status() << std::endl ;

                        if ( tracker.is_accel_set() )
                            ::std::cout << "    accel: " << tracker.get_accel().x << ", " << 
                                                            tracker.get_accel().y << ", " <<
                                                            tracker.get_accel().z << std::endl ;

                        if ( tracker.is_target_pos_set() )
                            ::std::cout << "    target pos: " << tracker.get_target_pos().x << ", " << 
                                                                 tracker.get_target_pos().y << ", " <<
                                                                 tracker.get_target_pos().z << std::endl ;

                        if ( tracker.is_timestamp_set() )
                            ::std::cout << "    timestamp: " << tracker.get_timestamp() << std::endl ;
                    }

                    ::std::cout << "-----------------------------------------------" << ::std::endl ;
                //} // skip

        }

        else {
            printf("\n");
            // sleep(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(1) );
        }



        // if ( socket_client.receive_message( msg , ::psn::MAX_UDP_PACKET_SIZE ) )
        // {

        
        std::this_thread::sleep_for(std::chrono::milliseconds(1) );

    }

    return 0;
}


