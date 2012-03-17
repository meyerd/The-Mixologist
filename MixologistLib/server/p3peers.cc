/****************************************************************
 *  Copyright 2010, Fair Use, Inc.
 *  Copyright 2004-8, Robert Fernie
 *
 *  This file is part of the Mixologist.
 *
 *  The Mixologist is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The Mixologist is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Mixologist; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/


#include "server/p3peers.h"
#include "server/server.h"
#include "pqi/connectivitymanager.h"
#include "pqi/authmgr.h"
#include <interface/init.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "pqi/authmgr.h"

p3Peers::p3Peers(QString &ownName)
    :ownName(ownName) {}

/* Peer Details (Net & Auth) */
std::string p3Peers::getOwnCertId() {
    return authMgr->OwnCertId();
}

unsigned int p3Peers::getOwnLibraryMixerId() {
    return authMgr->OwnLibraryMixerId();
}

QString p3Peers::getOwnName(){
    return ownName;
}

void p3Peers::getOnlineList(std::list<int> &ids) {
    connMgr->getOnlineList(ids);
}

void p3Peers::getSignedUpList(std::list<int> &ids) {
    connMgr->getSignedUpList(ids);
}

void p3Peers::getFriendList(std::list<int> &ids) {
    connMgr->getFriendList(ids);
}

bool p3Peers::isOnline(unsigned int librarymixer_id) {
    return connMgr->isOnline(librarymixer_id);
}

bool p3Peers::isFriend(unsigned int librarymixer_id) {
    return connMgr->isFriend(librarymixer_id);
}

bool p3Peers::getPeerDetails(unsigned int librarymixer_id, PeerDetails &d) {
    peerConnectState pcs;
    if (!connMgr->getPeerConnectState(librarymixer_id, pcs)) return false;
    d.id = pcs.id;
    d.librarymixer_id = pcs.librarymixer_id;
    d.name = pcs.name;

    /* fill from pcs */

    d.localAddr = inet_ntoa(pcs.localaddr.sin_addr);
    d.localPort = ntohs(pcs.localaddr.sin_port);
    d.extAddr   = inet_ntoa(pcs.serveraddr.sin_addr);
    d.extPort   = ntohs(pcs.serveraddr.sin_port);
    d.lastConnect   = pcs.lastcontact;

    /* Translate */

    if (pcs.state & PEER_S_CONNECTED)
        d.state = PEER_STATE_CONNECTED;
    else if (pcs.schedulednexttry != 0)
        d.state = PEER_STATE_WAITING_FOR_RETRY;
    else if (pcs.inConnAttempt)
        d.state = PEER_STATE_TRYING;
    else if (pcs.state & PEER_S_NO_CERT)
        d.state = PEER_STATE_NO_CERT;
    else d.state = PEER_STATE_OFFLINE;

    return true;
}

std::string p3Peers::findCertByLibraryMixerId(unsigned int librarymixer_id) {
    return authMgr->findCertByLibraryMixerId(librarymixer_id);
}

unsigned int p3Peers::findLibraryMixerByCertId(std::string cert_id) {
    return authMgr->findLibraryMixerByCertId(cert_id);
}

QString p3Peers::getPeerName(unsigned int librarymixer_id) {
    return connMgr->getPeerName(librarymixer_id);
}

/* Add/Remove Friends */
bool p3Peers::addUpdateFriend(unsigned int librarymixer_id, const QString &cert, const QString &name,
                              const QString &localIP, ushort localPort, const QString &externalIP, ushort externalPort) {
    return connMgr->addUpdateFriend(librarymixer_id, cert, name, localIP, localPort, externalIP, externalPort);
}

/* Network Stuff */
void p3Peers::connectAttempt(unsigned int librarymixer_id) {
    connMgr->retryConnect(librarymixer_id);
}

void p3Peers::connectAll() {
    connMgr->retryConnectAll();
}

PeerDetails::PeerDetails()
    :state(PEER_STATE_OFFLINE), lastConnect(0) {
    return;
}
