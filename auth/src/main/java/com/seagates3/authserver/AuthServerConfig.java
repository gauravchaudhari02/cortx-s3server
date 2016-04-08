/*
 * COPYRIGHT 2015 SEAGATE LLC
 *
 * THIS DRAWING/DOCUMENT, ITS SPECIFICATIONS, AND THE DATA CONTAINED
 * HEREIN, ARE THE EXCLUSIVE PROPERTY OF SEAGATE TECHNOLOGY
 * LIMITED, ISSUED IN STRICT CONFIDENCE AND SHALL NOT, WITHOUT
 * THE PRIOR WRITTEN PERMISSION OF SEAGATE TECHNOLOGY LIMITED,
 * BE REPRODUCED, COPIED, OR DISCLOSED TO A THIRD PARTY, OR
 * USED FOR ANY PURPOSE WHATSOEVER, OR STORED IN A RETRIEVAL SYSTEM
 * EXCEPT AS ALLOWED BY THE TERMS OF SEAGATE LICENSES AND AGREEMENTS.
 *
 * YOU SHOULD HAVE RECEIVED A COPY OF SEAGATE'S LICENSE ALONG WITH
 * THIS RELEASE. IF NOT PLEASE CONTACT A SEAGATE REPRESENTATIVE
 * http://www.seagate.com/contact
 *
 * Original author:  Arjun Hariharan <arjun.hariharan@seagate.com>
 * Original creation date: 23-Oct-2015
 */
package com.seagates3.authserver;

import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Properties;

/**
 * Store the auth server configuration properties like default endpoint and
 * server end points.
 */
public class AuthServerConfig {

    private static String samlMetadataFilePath;
    private static Properties authServerConfig;

    /**
     * Initialize default endpoint and s3 endpoints etc.
     *
     * @param authServerConfig Server configuration parameters.
     */
    public static void init(Properties authServerConfig) {
        AuthServerConfig.authServerConfig = authServerConfig;

        setSamlMetadataFile(authServerConfig.getProperty(
                "samlMetadataFileName"));
    }

    /**
     * Return the end points of S3-Auth server.
     *
     * @return server endpoints.
     */
    public static String[] getEndpoints() {
        return authServerConfig.getProperty("s3Endpoints").split(",");
    }

    /**
     * Return the default end point of S3-Auth server.
     *
     * @return default endpoint.
     */
    public static String getDefaultEndpoint() {
        return authServerConfig.getProperty("defaultEndpoint");
    }

    /**
     * @return Path of SAML metadata file.
     */
    public static String getSAMLMetadataFilePath() {
        return samlMetadataFilePath;
    }

    public static int getHttpPort() {
        return Integer.parseInt(authServerConfig.getProperty("httpPort"));
    }

    public static int getHttpsPort() {
        return Integer.parseInt(authServerConfig.getProperty("httpsPort"));
    }

    public static String getKeyStoreName() {
        return authServerConfig.getProperty("s3KeystoreName");
    }

    public static String getKeyStorePassword() {
        return authServerConfig.getProperty("s3KeyStorePassword");
    }

    public static String getKeyPassword() {
        return authServerConfig.getProperty("s3KeyPassword");
    }

    public static Boolean isHttpsEnabled() {
        return authServerConfig.getProperty("https")
                .compareToIgnoreCase("true") == 0;
    }

    public static String getDataSource() {
        return authServerConfig.getProperty("dataSource");
    }

    public static String getLdapHost() {
        return authServerConfig.getProperty("ldapHost");
    }

    public static int getLdapPort() {
        return Integer.parseInt(authServerConfig.getProperty("ldapPort"));
    }

    public static int getLdapMaxConnections() {
        return Integer.parseInt(authServerConfig.getProperty("maxCons"));
    }

    public static int getLdapMaxSharedConnections() {
        return Integer.parseInt(authServerConfig.getProperty("maxSharedCons"));
    }

    public static String getLdapLoginDN() {
        return authServerConfig.getProperty("ldapLoginDN");
    }

    public static String getLdapLoginPassword() {
        return authServerConfig.getProperty("ldapLoginPW");
    }

    public static String getConsoleURL() {
        return authServerConfig.getProperty("consoleURL");
    }

    public static String getLogConfigFile() {
        return authServerConfig.getProperty("logConfigFile");
    }

    public static String getLogLevel() {
        return authServerConfig.getProperty("logLevel");
    }

    /**
     * Set the SAML Metadata file Path.
     *
     * @param fileName Name of the metadata file.
     */
    private static void setSamlMetadataFile(String fileName) {
        Path filePath = Paths.get("", "resources", "static", fileName);
        samlMetadataFilePath = filePath.toString();
    }
}